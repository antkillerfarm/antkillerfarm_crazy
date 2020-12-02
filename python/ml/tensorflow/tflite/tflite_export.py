import os
import numpy as np
import flatbuffers
import tflite_v3.Model
import tflite_v3.Buffer
import tflite_v3.Tensor
import tflite_v3.QuantizationParameters
import tflite_v3.SubGraph
import tflite_v3.Operator
import tflite_v3.OperatorCode
import tflite_v3.BuiltinOperator


input_name = 'resize.tflite'
output_name = 'resize_1.tflite'
TFLITE_SCHEMA_VERSION = 3
cur_dir = os.path.dirname(os.path.abspath(__file__))

def _overwrite_flatbuffers_buffer(model, buffer_idx, new_contents):
    orig_buffer = model.Buffers(buffer_idx)
    # NB. Update this to directly manipulate `serialized_model` if this view becomes unwriteable
    orig_buffer.DataAsNumpy()[:] = new_contents.astype(np.uint8).flatten()

def test_import(input_name):
    path = os.path.join(cur_dir, input_name)
    with open(path, 'rb') as f:
        buf = f.read()
        model = tflite_v3.Model.Model.GetRootAsModel(buf, 0)
    assert(model.Version() == 3)
    print("Subgraphs: {}".format(model.SubgraphsLength()))
    subgraph = model.Subgraphs(0)
    print("Tensors:")
    for i in range(subgraph.TensorsLength()):
        t = subgraph.Tensors(i)
        print("{} $$ {}".format(t.Name(), t.Type()))
    #t = subgraph.Tensors(1) # 'resize/size'
    #buffer_idx = t.Buffer()
    #buffer = model.Buffers(buffer_idx)
    #arr = buffer.DataAsNumpy()

    print("Operators:")
    for i in range(subgraph.OperatorsLength()):
        op = subgraph.Operators(i)
        opcode = model.OperatorCodes(op.OpcodeIndex()).BuiltinCode()
        print("{} $$ {}".format(op.OutputsLength(), opcode))
    #path = os.path.join(cur_dir, output_name)
    #open(path, 'wb').write(buf)


def build_mock_flatbuffer_model():
  """Creates a flatbuffer containing an example model."""
  builder = flatbuffers.Builder(1024 * 1024)

  tflite_v3.Buffer.BufferStart(builder)
  buffer0_offset = tflite_v3.Buffer.BufferEnd(builder)

  tflite_v3.Buffer.BufferStartDataVector(builder, 10)
  builder.PrependUint8(9)
  builder.PrependUint8(8)
  builder.PrependUint8(7)
  builder.PrependUint8(6)
  builder.PrependUint8(5)
  builder.PrependUint8(4)
  builder.PrependUint8(3)
  builder.PrependUint8(2)
  builder.PrependUint8(1)
  builder.PrependUint8(0)
  buffer1_data_offset = builder.EndVector(10)
  tflite_v3.Buffer.BufferStart(builder)
  tflite_v3.Buffer.BufferAddData(builder, buffer1_data_offset)
  buffer1_offset = tflite_v3.Buffer.BufferEnd(builder)

  tflite_v3.Buffer.BufferStart(builder)
  buffer2_offset = tflite_v3.Buffer.BufferEnd(builder)

  tflite_v3.Model.ModelStartBuffersVector(builder, 3)
  builder.PrependUOffsetTRelative(buffer2_offset)
  builder.PrependUOffsetTRelative(buffer1_offset)
  builder.PrependUOffsetTRelative(buffer0_offset)
  buffers_offset = builder.EndVector(3)

  string0_offset = builder.CreateString('input_tensor')
  tflite_v3.Tensor.TensorStartShapeVector(builder, 3)
  builder.PrependInt32(1)
  builder.PrependInt32(2)
  builder.PrependInt32(5)
  shape0_offset = builder.EndVector(3)
  tflite_v3.Tensor.TensorStart(builder)
  tflite_v3.Tensor.TensorAddName(builder, string0_offset)
  tflite_v3.Tensor.TensorAddShape(builder, shape0_offset)
  tflite_v3.Tensor.TensorAddType(builder, 0)
  tflite_v3.Tensor.TensorAddBuffer(builder, 0)
  tensor0_offset = tflite_v3.Tensor.TensorEnd(builder)

  tflite_v3.QuantizationParameters.QuantizationParametersStartMinVector(builder, 5)
  builder.PrependFloat32(0.5)
  builder.PrependFloat32(2.0)
  builder.PrependFloat32(5.0)
  builder.PrependFloat32(10.0)
  builder.PrependFloat32(20.0)
  quant1_min_offset = builder.EndVector(5)

  tflite_v3.QuantizationParameters.QuantizationParametersStartMaxVector(builder, 5)
  builder.PrependFloat32(10.0)
  builder.PrependFloat32(20.0)
  builder.PrependFloat32(-50.0)
  builder.PrependFloat32(1.0)
  builder.PrependFloat32(2.0)
  quant1_max_offset = builder.EndVector(5)

  tflite_v3.QuantizationParameters.QuantizationParametersStartScaleVector(builder, 5)
  builder.PrependFloat32(3.0)
  builder.PrependFloat32(4.0)
  builder.PrependFloat32(5.0)
  builder.PrependFloat32(6.0)
  builder.PrependFloat32(7.0)
  quant1_scale_offset = builder.EndVector(5)

  tflite_v3.QuantizationParameters.QuantizationParametersStartZeroPointVector(builder, 5)
  builder.PrependInt64(1)
  builder.PrependInt64(2)
  builder.PrependInt64(3)
  builder.PrependInt64(-1)
  builder.PrependInt64(-2)
  quant1_zero_point_offset = builder.EndVector(5)

  tflite_v3.QuantizationParameters.QuantizationParametersStart(builder)
  tflite_v3.QuantizationParameters.QuantizationParametersAddMin(builder, quant1_min_offset)
  tflite_v3.QuantizationParameters.QuantizationParametersAddMax(builder, quant1_max_offset)
  tflite_v3.QuantizationParameters.QuantizationParametersAddScale(builder, quant1_scale_offset)
  tflite_v3.QuantizationParameters.QuantizationParametersAddZeroPoint(builder,
                                               quant1_zero_point_offset)
  quantization1_offset = tflite_v3.QuantizationParameters.QuantizationParametersEnd(builder)

  string1_offset = builder.CreateString('constant_tensor')
  tflite_v3.Tensor.TensorStartShapeVector(builder, 3)
  builder.PrependInt32(1)
  builder.PrependInt32(2)
  builder.PrependInt32(5)
  shape1_offset = builder.EndVector(3)
  tflite_v3.Tensor.TensorStart(builder)
  tflite_v3.Tensor.TensorAddName(builder, string1_offset)
  tflite_v3.Tensor.TensorAddShape(builder, shape1_offset)
  tflite_v3.Tensor.TensorAddType(builder, 0)
  tflite_v3.Tensor.TensorAddBuffer(builder, 1)
  tflite_v3.Tensor.TensorAddQuantization(builder, quantization1_offset)
  tensor1_offset = tflite_v3.Tensor.TensorEnd(builder)

  string2_offset = builder.CreateString('output_tensor')
  tflite_v3.Tensor.TensorStartShapeVector(builder, 3)
  builder.PrependInt32(1)
  builder.PrependInt32(2)
  builder.PrependInt32(5)
  shape2_offset = builder.EndVector(3)
  tflite_v3.Tensor.TensorStart(builder)
  tflite_v3.Tensor.TensorAddName(builder, string2_offset)
  tflite_v3.Tensor.TensorAddShape(builder, shape2_offset)
  tflite_v3.Tensor.TensorAddType(builder, 0)
  tflite_v3.Tensor.TensorAddBuffer(builder, 2)
  tensor2_offset = tflite_v3.Tensor.TensorEnd(builder)

  tflite_v3.SubGraph.SubGraphStartTensorsVector(builder, 3)
  builder.PrependUOffsetTRelative(tensor2_offset)
  builder.PrependUOffsetTRelative(tensor1_offset)
  builder.PrependUOffsetTRelative(tensor0_offset)
  tensors_offset = builder.EndVector(3)

  tflite_v3.SubGraph.SubGraphStartInputsVector(builder, 1)
  builder.PrependInt32(0)
  inputs_offset = builder.EndVector(1)

  tflite_v3.SubGraph.SubGraphStartOutputsVector(builder, 1)
  builder.PrependInt32(2)
  outputs_offset = builder.EndVector(1)

  tflite_v3.OperatorCode.OperatorCodeStart(builder)
  tflite_v3.OperatorCode.OperatorCodeAddBuiltinCode(builder, tflite_v3.BuiltinOperator.BuiltinOperator.ADD)
  #tflite_v3.OperatorCode.OperatorCodeAddVersion(builder, 1)
  code_offset = tflite_v3.OperatorCode.OperatorCodeEnd(builder)

  tflite_v3.Model.ModelStartOperatorCodesVector(builder, 1)
  builder.PrependUOffsetTRelative(code_offset)
  codes_offset = builder.EndVector(1)

  tflite_v3.Operator.OperatorStartInputsVector(builder, 2)
  builder.PrependInt32(0)
  builder.PrependInt32(1)
  op_inputs_offset = builder.EndVector(2)

  tflite_v3.Operator.OperatorStartOutputsVector(builder, 1)
  builder.PrependInt32(2)
  op_outputs_offset = builder.EndVector(1)

  tflite_v3.Operator.OperatorStart(builder)
  tflite_v3.Operator.OperatorAddOpcodeIndex(builder, 0)
  tflite_v3.Operator.OperatorAddInputs(builder, op_inputs_offset)
  tflite_v3.Operator.OperatorAddOutputs(builder, op_outputs_offset)
  op_offset = tflite_v3.Operator.OperatorEnd(builder)

  tflite_v3.SubGraph.SubGraphStartOperatorsVector(builder, 1)
  builder.PrependUOffsetTRelative(op_offset)
  ops_offset = builder.EndVector(1)

  string3_offset = builder.CreateString('subgraph_name')
  tflite_v3.SubGraph.SubGraphStart(builder)
  tflite_v3.SubGraph.SubGraphAddName(builder, string3_offset)
  tflite_v3.SubGraph.SubGraphAddTensors(builder, tensors_offset)
  tflite_v3.SubGraph.SubGraphAddInputs(builder, inputs_offset)
  tflite_v3.SubGraph.SubGraphAddOutputs(builder, outputs_offset)
  tflite_v3.SubGraph.SubGraphAddOperators(builder, ops_offset)
  subgraph_offset = tflite_v3.SubGraph.SubGraphEnd(builder)

  tflite_v3.Model.ModelStartSubgraphsVector(builder, 1)
  builder.PrependUOffsetTRelative(subgraph_offset)
  subgraphs_offset = builder.EndVector(1)

  string4_offset = builder.CreateString('model_description')
  tflite_v3.Model.ModelStart(builder)
  tflite_v3.Model.ModelAddVersion(builder, TFLITE_SCHEMA_VERSION)
  tflite_v3.Model.ModelAddOperatorCodes(builder, codes_offset)
  tflite_v3.Model.ModelAddSubgraphs(builder, subgraphs_offset)
  tflite_v3.Model.ModelAddDescription(builder, string4_offset)
  tflite_v3.Model.ModelAddBuffers(builder, buffers_offset)
  model_offset = tflite_v3.Model.ModelEnd(builder)
  builder.Finish(model_offset)
  model = builder.Output()

  return model

def build_mock_flatbuffer_model1():
  """Creates a flatbuffer containing an example model."""
  builder = flatbuffers.Builder(1024 * 1024)

  tflite_v3.Buffer.BufferStart(builder)
  buffer0_offset = tflite_v3.Buffer.BufferEnd(builder)

  tflite_v3.Buffer.BufferStartDataVector(builder, 10)
  builder.PrependUint8(9)
  builder.PrependUint8(8)
  builder.PrependUint8(7)
  builder.PrependUint8(6)
  builder.PrependUint8(5)
  builder.PrependUint8(4)
  builder.PrependUint8(3)
  builder.PrependUint8(2)
  builder.PrependUint8(1)
  builder.PrependUint8(0)
  buffer1_data_offset = builder.EndVector(10)
  tflite_v3.Buffer.BufferStart(builder)
  tflite_v3.Buffer.BufferAddData(builder, buffer1_data_offset)
  buffer1_offset = tflite_v3.Buffer.BufferEnd(builder)

  tflite_v3.Buffer.BufferStart(builder)
  buffer2_offset = tflite_v3.Buffer.BufferEnd(builder)

  tflite_v3.Model.ModelStartBuffersVector(builder, 3)
  builder.PrependUOffsetTRelative(buffer2_offset)
  builder.PrependUOffsetTRelative(buffer1_offset)
  builder.PrependUOffsetTRelative(buffer0_offset)
  buffers_offset = builder.EndVector(3)

  string0_offset = builder.CreateString('input_tensor')
  tflite_v3.Tensor.TensorStartShapeVector(builder, 3)
  builder.PrependInt32(1)
  builder.PrependInt32(2)
  builder.PrependInt32(5)
  shape0_offset = builder.EndVector(3)
  tflite_v3.Tensor.TensorStart(builder)
  tflite_v3.Tensor.TensorAddName(builder, string0_offset)
  tflite_v3.Tensor.TensorAddShape(builder, shape0_offset)
  tflite_v3.Tensor.TensorAddType(builder, 0)
  tflite_v3.Tensor.TensorAddBuffer(builder, 0)
  tensor0_offset = tflite_v3.Tensor.TensorEnd(builder)

  tflite_v3.QuantizationParameters.QuantizationParametersStartMinVector(builder, 5)
  builder.PrependFloat32(0.5)
  builder.PrependFloat32(2.0)
  builder.PrependFloat32(5.0)
  builder.PrependFloat32(10.0)
  builder.PrependFloat32(20.0)
  quant1_min_offset = builder.EndVector(5)

  tflite_v3.QuantizationParameters.QuantizationParametersStartMaxVector(builder, 5)
  builder.PrependFloat32(10.0)
  builder.PrependFloat32(20.0)
  builder.PrependFloat32(-50.0)
  builder.PrependFloat32(1.0)
  builder.PrependFloat32(2.0)
  quant1_max_offset = builder.EndVector(5)

  tflite_v3.QuantizationParameters.QuantizationParametersStartScaleVector(builder, 5)
  builder.PrependFloat32(3.0)
  builder.PrependFloat32(4.0)
  builder.PrependFloat32(5.0)
  builder.PrependFloat32(6.0)
  builder.PrependFloat32(7.0)
  quant1_scale_offset = builder.EndVector(5)

  tflite_v3.QuantizationParameters.QuantizationParametersStartZeroPointVector(builder, 5)
  builder.PrependInt64(1)
  builder.PrependInt64(2)
  builder.PrependInt64(3)
  builder.PrependInt64(-1)
  builder.PrependInt64(-2)
  quant1_zero_point_offset = builder.EndVector(5)

  tflite_v3.QuantizationParameters.QuantizationParametersStart(builder)
  tflite_v3.QuantizationParameters.QuantizationParametersAddMin(builder, quant1_min_offset)
  tflite_v3.QuantizationParameters.QuantizationParametersAddMax(builder, quant1_max_offset)
  tflite_v3.QuantizationParameters.QuantizationParametersAddScale(builder, quant1_scale_offset)
  tflite_v3.QuantizationParameters.QuantizationParametersAddZeroPoint(builder,
                                               quant1_zero_point_offset)
  quantization1_offset = tflite_v3.QuantizationParameters.QuantizationParametersEnd(builder)

  string1_offset = builder.CreateString('constant_tensor')
  tflite_v3.Tensor.TensorStartShapeVector(builder, 3)
  builder.PrependInt32(1)
  builder.PrependInt32(2)
  builder.PrependInt32(5)
  shape1_offset = builder.EndVector(3)
  tflite_v3.Tensor.TensorStart(builder)
  tflite_v3.Tensor.TensorAddName(builder, string1_offset)
  tflite_v3.Tensor.TensorAddShape(builder, shape1_offset)
  tflite_v3.Tensor.TensorAddType(builder, 0)
  tflite_v3.Tensor.TensorAddBuffer(builder, 1)
  tflite_v3.Tensor.TensorAddQuantization(builder, quantization1_offset)
  tensor1_offset = tflite_v3.Tensor.TensorEnd(builder)

  string2_offset = builder.CreateString('output_tensor')
  tflite_v3.Tensor.TensorStartShapeVector(builder, 3)
  builder.PrependInt32(1)
  builder.PrependInt32(2)
  builder.PrependInt32(5)
  shape2_offset = builder.EndVector(3)
  tflite_v3.Tensor.TensorStart(builder)
  tflite_v3.Tensor.TensorAddName(builder, string2_offset)
  tflite_v3.Tensor.TensorAddShape(builder, shape2_offset)
  tflite_v3.Tensor.TensorAddType(builder, 0)
  tflite_v3.Tensor.TensorAddBuffer(builder, 2)
  tensor2_offset = tflite_v3.Tensor.TensorEnd(builder)

  tflite_v3.SubGraph.SubGraphStartTensorsVector(builder, 3)
  builder.PrependUOffsetTRelative(tensor2_offset)
  builder.PrependUOffsetTRelative(tensor1_offset)
  builder.PrependUOffsetTRelative(tensor0_offset)
  tensors_offset = builder.EndVector(3)

  tflite_v3.SubGraph.SubGraphStartInputsVector(builder, 1)
  builder.PrependInt32(0)
  inputs_offset = builder.EndVector(1)

  tflite_v3.SubGraph.SubGraphStartOutputsVector(builder, 1)
  builder.PrependInt32(2)
  outputs_offset = builder.EndVector(1)

  tflite_v3.OperatorCode.OperatorCodeStart(builder)
  tflite_v3.OperatorCode.OperatorCodeAddBuiltinCode(builder, tflite_v3.BuiltinOperator.BuiltinOperator.ADD)
  #tflite_v3.OperatorCode.OperatorCodeAddVersion(builder, 1)
  code_offset = tflite_v3.OperatorCode.OperatorCodeEnd(builder)

  tflite_v3.Model.ModelStartOperatorCodesVector(builder, 1)
  builder.PrependUOffsetTRelative(code_offset)
  codes_offset = builder.EndVector(1)

  tflite_v3.Operator.OperatorStartInputsVector(builder, 2)
  builder.PrependInt32(0)
  builder.PrependInt32(1)
  op_inputs_offset = builder.EndVector(2)

  tflite_v3.Operator.OperatorStartOutputsVector(builder, 1)
  builder.PrependInt32(2)
  op_outputs_offset = builder.EndVector(1)

  tflite_v3.Operator.OperatorStart(builder)
  tflite_v3.Operator.OperatorAddOpcodeIndex(builder, 0)
  tflite_v3.Operator.OperatorAddInputs(builder, op_inputs_offset)
  tflite_v3.Operator.OperatorAddOutputs(builder, op_outputs_offset)
  op_offset = tflite_v3.Operator.OperatorEnd(builder)

  tflite_v3.SubGraph.SubGraphStartOperatorsVector(builder, 1)
  builder.PrependUOffsetTRelative(op_offset)
  ops_offset = builder.EndVector(1)

  string3_offset = builder.CreateString('subgraph_name')
  tflite_v3.SubGraph.SubGraphStart(builder)
  tflite_v3.SubGraph.SubGraphAddName(builder, string3_offset)
  tflite_v3.SubGraph.SubGraphAddTensors(builder, tensors_offset)
  tflite_v3.SubGraph.SubGraphAddInputs(builder, inputs_offset)
  tflite_v3.SubGraph.SubGraphAddOutputs(builder, outputs_offset)
  tflite_v3.SubGraph.SubGraphAddOperators(builder, ops_offset)
  subgraph_offset = tflite_v3.SubGraph.SubGraphEnd(builder)

  tflite_v3.Model.ModelStartSubgraphsVector(builder, 1)
  builder.PrependUOffsetTRelative(subgraph_offset)
  subgraphs_offset = builder.EndVector(1)

  string4_offset = builder.CreateString('model_description')
  tflite_v3.Model.ModelStart(builder)
  tflite_v3.Model.ModelAddVersion(builder, TFLITE_SCHEMA_VERSION)
  tflite_v3.Model.ModelAddOperatorCodes(builder, codes_offset)
  tflite_v3.Model.ModelAddSubgraphs(builder, subgraphs_offset)
  tflite_v3.Model.ModelAddDescription(builder, string4_offset)
  tflite_v3.Model.ModelAddBuffers(builder, buffers_offset)
  model_offset = tflite_v3.Model.ModelEnd(builder)
  builder.Finish(model_offset)
  model = builder.Output()

  return model

test_import(input_name)
path = os.path.join(cur_dir, output_name)
model = build_mock_flatbuffer_model()
open(path, 'wb').write(model)
print("ok")
