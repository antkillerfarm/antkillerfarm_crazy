template <typename T>
Constant QuantizeData(std::vector<float>& in, std::vector<float>& scale,
  std::vector<int32_t>& zp, DataType out_dtype, std::vector<int64_t> shape) {
  std::vector<T> out;
  for (auto val : in) {
    val = val / scale[0] + zp[0];
    out.push_back(static_cast<T>(val));
  }
  return MakeConstantTensor(out_dtype, shape, out);
}

class QnnQuantizeConstFold : public DFPatternRewrite {
 public:
  QnnQuantizeConstFold() {
    data_pat_ = IsConstant();
    pattern_ = IsOp("qnn.quantize")({data_pat_, IsConstant(), IsConstant()});
  }

  Expr Callback(const Expr& pre, const Expr& post,
                const Map<DFPattern, Array<Expr>>& node_map) const override {
    // const CallNode* call = pre.as<CallNode>();
    // const TensorTypeNode* input = call->args[0]->checked_type().as<TensorTypeNode>();
    const TensorTypeNode* output = pre->checked_type().as<TensorTypeNode>();
    std::vector<float> val_input;
    std::vector<float> val_scale;
    std::vector<int32_t> val_zp;
    std::vector<int64_t> shape;
    auto call_pre = Downcast<Call>(pre);
    runtime::NDArray data = Downcast<Constant>(call_pre->args[0])->data;
    for (int i = 0; i < data->ndim; i++) {
      shape.push_back(data->shape[i]);
    }

    AsConstant(call_pre->args[0], val_input);
    AsConstant(call_pre->args[1], val_scale);
    AsConstant(call_pre->args[2], val_zp);

    if (output->dtype == DataType::Int(8)) {
      return QuantizeData<int8_t>(val_input, val_scale, val_zp, output->dtype, shape);
    } else if (output->dtype == DataType::Int(32)) {
      return QuantizeData<int32_t>(val_input, val_scale, val_zp, output->dtype, shape);
    }

    // LOG(INFO) << "QnnQuantizeConstFold : " << input->dtype << " :: " << output->dtype
    //   << " :: " << call_pre->args[2]->checked_type().as<TensorTypeNode>()->dtype
    //   << " :: " << AsText(node_map[data_pat_][0], false);
    return post;
  }

 protected:
  DFPattern data_pat_;
};

class RemoveClipAfterRequantize : public DFPatternRewrite {
 public:
  RemoveClipAfterRequantize() {
    x_ = IsWildcard();
    for (int i =0; i < 4; i++){
      const_[i] = IsConstant();
    }
    requantize_ = IsOp("qnn.requantize")({x_, const_[0], const_[1], const_[2], const_[3]});
    clip_ = IsOp("clip")({requantize_});
    pattern_ = IsOp("cast")({clip_});
  }

  Expr Callback(const Expr& pre, const Expr& post,
                const Map<DFPattern, Array<Expr>>& node_map) const override {
    auto x = node_map[x_][0];
    auto requantize = node_map[requantize_][0];
    auto pattern = node_map[pattern_][0];
    const CallNode* call_requantize = requantize.as<CallNode>();
    const CallNode* call_cast = pattern.as<CallNode>();
    Expr constant[4];
    for (int i =0; i < 4; i++){
      constant[i] = node_map[const_[i]][0];
    }
    // LOG(INFO) << "RemoveClipAfterRequantize : " << call_requantize->type_args
    //   << " :: " << call_requantize->span
    //   << " :: " << call_requantize->attrs.as<qnn::RequantizeAttrs>()->out_dtype
    //   << " :: " << call_cast->attrs.as<CastAttrs>()->dtype;
    auto attrs = make_object<qnn::RequantizeAttrs>();
    auto attrs_requantize = call_requantize->attrs.as<qnn::RequantizeAttrs>();
    attrs->axis = attrs_requantize->axis;
    attrs->rounding = attrs_requantize->rounding;
    attrs->out_dtype = call_cast->attrs.as<CastAttrs>()->dtype;
    return Call(call_requantize->op, {x, constant[0], constant[1], constant[2], constant[3]},
      Attrs(attrs), {});
    // return post;
  }

 protected:
  DFPattern x_;
  DFPattern const_[4];
  DFPattern requantize_;
  DFPattern clip_;
};
