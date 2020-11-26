#!/usr/bin/env python3
import tensorflow as tf
from modules import Imagenet
from modules import Model_set

EXCEPT_MODELS = ['vgg16', 'vgg19', 'nasNetLarge' ]
IMAGE_COUNT_FOR_REPRESENTATIVE = 100
TFLITE_EXPERIMENTAL_CONVERTER = True
FORCE_FULL_QUANT = True

def quantize_tflite(keras_model, save_path, repr_dataset_gen_fn, force_full_quant=False, enable_experimental_converter=False):
    #outputs = ["Conv1_relu", "predictions"]
    #cloning Keras model with debug outputs
    outputs = []
    outputs_name = ["predictions", "Conv1_pad", "Conv1_relu"]
    for output_name in outputs_name:
        outputs.append(keras_model.get_layer(output_name).output)
    model_debug = tf.keras.Model(inputs=keras_model.inputs, outputs=outputs)
    #converter = tf.lite.TFLiteConverter.from_keras_model(keras_model)
    converter = tf.lite.TFLiteConverter.from_keras_model(model_debug)
    converter.experimental_new_converter = TFLITE_EXPERIMENTAL_CONVERTER
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.representative_dataset = repr_dataset_gen_fn

    if force_full_quant:
        print('Forcing full quant')
        converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
        converter.inference_input_type = tf.uint8
        converter.inference_output_type = tf.uint8

    tflite_model = converter.convert()

    with open(save_path, 'wb') as f:
        f.write(tflite_model)


if __name__ == '__main__':
    ms = Model_set.ModelSet()
    for name in ms.models:
        if name in EXCEPT_MODELS:
            print('Skipping model {}'.format(name))
            continue
        ms.create_dir(name, tflite=True)

        print('Quantizing {}'.format(name))
        mi = ms.models[name]
        keras_model = tf.keras.models.load_model(ms.gen_path_keras(name))
        im_height = keras_model.input_shape[1]
        im_width = keras_model.input_shape[2]

        imageGen = Imagenet.ImagenetGenerator(mi.package.preprocess_input, im_height, im_width, num_images=IMAGE_COUNT_FOR_REPRESENTATIVE)

        quantize_tflite(keras_model,
                        ms.get_path_tflite_quant(name, FORCE_FULL_QUANT),
                        imageGen.gen_repr_dataset,
                        force_full_quant=FORCE_FULL_QUANT,
                        enable_experimental_converter=TFLITE_EXPERIMENTAL_CONVERTER)
