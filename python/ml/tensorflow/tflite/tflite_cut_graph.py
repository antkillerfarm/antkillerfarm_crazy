from tensorflow.lite.python import schema_py_generated as schema_fb
import flatbuffers
import tflite
import tensorflow as tf

def OutputsOffset(subgraph, j):
    o = flatbuffers.number_types.UOffsetTFlags.py_type(subgraph._tab.Offset(8))
    if o != 0:
        a = subgraph._tab.Vector(o)
        return a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 4)
    return 0

def InputsOffset(subgraph, j):
    o = flatbuffers.number_types.UOffsetTFlags.py_type(subgraph._tab.Offset(6))
    if o != 0:
        a = subgraph._tab.Vector(o)
        return a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 4)
    return 0

def buffer_change_input_tensor_to(model_buffer, new_tensor_i):
    
    root = schema_fb.Model.GetRootAsModel(model_buffer, 0)
    input_tensor_index_offset = InputsOffset(root.Subgraphs(0), 0)
    print("buffer_change_input_tensor_to",input_tensor_index_offset)

    # Flatbuffer scalars are stored in little-endian.
    new_tensor_i_bytes = bytes([
    new_tensor_i & 0x000000FF, \
    (new_tensor_i & 0x0000FF00) >> 8, \
    (new_tensor_i & 0x00FF0000) >> 16, \
    (new_tensor_i & 0xFF000000) >> 24 \
    ])
    print("new_tensor_i",new_tensor_i)

    # Replace the 4 bytes corresponding to the first output tensor index
    return model_buffer[:input_tensor_index_offset] + new_tensor_i_bytes + model_buffer[input_tensor_index_offset + 4:]

def buffer_change_output_tensor_to(model_buffer, new_tensor_i):
    
    root = schema_fb.Model.GetRootAsModel(model_buffer, 0)
    output_tensor_index_offset = OutputsOffset(root.Subgraphs(0), 0)
    print("buffer_change_output_tensor_to",output_tensor_index_offset)

    # Flatbuffer scalars are stored in little-endian.
    new_tensor_i_bytes = bytes([
    new_tensor_i & 0x000000FF, \
    (new_tensor_i & 0x0000FF00) >> 8, \
    (new_tensor_i & 0x00FF0000) >> 16, \
    (new_tensor_i & 0xFF000000) >> 24 \
    ])
    print("new_tensor_i",new_tensor_i)

    # Replace the 4 bytes corresponding to the first output tensor index
    return model_buffer[:output_tensor_index_offset] + new_tensor_i_bytes + model_buffer[output_tensor_index_offset + 4:]

path = "/home/ubuser/my/work/demo/"

with open(path + 'deeplabv3_mnv2_pascal_quant.tflite', 'rb') as f:
    model_buffer = f.read()

op_idx = 4
in_tensor_idx = 2
out_tensor_idx = 180 # use interpreter.get_tensor_details() to print tensor_idx
model_buffer = buffer_change_output_tensor_to(model_buffer, out_tensor_idx)
model_buffer = buffer_change_input_tensor_to(model_buffer, in_tensor_idx)

with open(path + 'operation_' + str(op_idx) + '_in_' + str(in_tensor_idx) + '_out_'+str(out_tensor_idx) + '.tflite', 'wb') as g:
    g.write(model_buffer)
