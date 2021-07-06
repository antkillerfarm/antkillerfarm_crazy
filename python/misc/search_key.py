#!/usr/bin/python
# -*- coding: utf-8 -*-

def get_model(model_list, model_name):
    for model in model_list:
        if (model['name'] == model_name):
            return model

model_list = [
    {'name': 'efficientnet-edgetpu-S_quant.tflite',
     'shape': (1, 224, 224, 3),
     'input_tensor_name': 'images'},
    {'name': 'deeplabv3_mnv2_pascal_quant.tflite',
     'shape': (1, 513, 513, 3),
     'input_tensor_name': 'MobilenetV2/MobilenetV2/input'},
]

model = get_model(model_list, "efficientnet-edgetpu-S_quant.tflite")
print(model)
