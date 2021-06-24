import tensorflow as tf

interpreter = tf.lite.Interpreter(model_path='./resize.tflite')
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

print(input_details)
print(output_details)
