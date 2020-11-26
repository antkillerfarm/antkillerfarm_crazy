#!/usr/bin/env python3
'''
Script generates model from keras.application (the set is derined in /modules/Model_set.py) and store the in:
- keras format
- TFLite format
'''
from modules import Model_set
import tensorflow as tf
import os

GEN_TFLITE_FLOAT = True
TFLITE_EXPERIMENTAL_CONVERTER = True
EXCEPT_MODELS = ['vgg16', 'vgg19', 'nasNetLarge' ]

if __name__ == '__main__':
    print(tf.version.VERSION)
    ms = Model_set.ModelSet()
    for name in ms.models:
        if name in EXCEPT_MODELS:
            print('Skipping model {}'.format(name))
            continue
        else:
            print("Generating model {}".format(name))

        #Get Model Instance structure and create directory
        mi = ms.models[name]
        ms.create_dir(name)

        print('\tKeras verzion')
        model = mi.model()
        model.save(os.path.join(ms.gen_path(name), ms.gen_fn_keras(name)))

        if GEN_TFLITE_FLOAT:
            print('\tTFLite Float Verzion')
            converter = tf.lite.TFLiteConverter.from_keras_model(model)
            converter.experimental_new_converter = TFLITE_EXPERIMENTAL_CONVERTER
            try:
                tf_model = converter.convert()
            except:
                print('\tConversion to tflite FAILED')
            with open(os.path.join(ms.gen_path(name), ms.gen_fn_tflite_float(name)), 'wb') as f:
                f.write(tf_model)

        print('...Done')
