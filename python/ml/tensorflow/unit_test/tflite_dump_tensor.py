import tensorflow as tf
from PIL import Image
import numpy as np

# Load TFLite model and allocate tensors.
#interpreter = tf.lite.Interpreter(model_path="mobilenetV2quant-full.tflite")
interpreter = tf.lite.Interpreter(model_path="mobilenetV2.tflite")
interpreter.allocate_tensors()

# Get input and output tensors.
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()
tensor_details = interpreter.get_tensor_details()

# Set input
img = np.array(Image.open('space_shuttle_224x224.jpg'))
img = np.reshape(img, [1, 224, 224, 3])
img = img.astype(np.float32)
img = img - 127.5
img = img / 256.0
if 0:
    scale = input_details[0]['quantization'][0]
    zero_point = input_details[0]['quantization'][1]
    img = img * scale - zero_point
    img = img / scale
    img = img.astype(np.int8)
interpreter.set_tensor(input_details[0]['index'], img)

# invoke
interpreter.invoke()

# Get output
output_data = interpreter.get_tensor(output_details[0]['index'])
if 0:
    scale = output_details[0]['quantization'][0]
    zero_point = output_details[0]['quantization'][1]
    output_data = output_data - zero_point
    output_data = output_data * scale
ind = np.argsort(-output_data)

for i in range(5):
    print("{}: {}".format(ind[0][i], output_data[0][ind[0][i]]))

print("OK")
