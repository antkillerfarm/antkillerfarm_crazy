import tensorflow as tf

start_shape = (33, 33)
resize_shape = (513, 513)
upsample_size = (resize_shape[0] // start_shape[0], resize_shape[1] // start_shape[1])

input_layer = tf.keras.layers.Input(shape=(start_shape[0], start_shape[1], 1), batch_size=1)
x = tf.keras.layers.UpSampling2D(size=upsample_size, interpolation='bilinear')(input_layer)
model = tf.keras.models.Model(input_layer, x)

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()
open('resize_2.tflite', 'wb').write(tflite_model)
