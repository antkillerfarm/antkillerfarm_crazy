import onnx
from onnx import helper
model = onnx.load('xx.onnx')

intermediate_tensor_name = 'xxx'
model_path = 'xx_new.onnx'
intermediate_layer_value_info = helper.ValueInfoProto()
intermediate_layer_value_info.name = intermediate_tensor_name
model.graph.output.append(intermediate_layer_value_info)
onnx.save(model, model_path)
