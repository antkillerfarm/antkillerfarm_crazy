
from  tensorflow.keras import applications
import os

# from tensorflow_core.python.kernel_tests.signal.test_util import tflite_convert

TF_LITE_CONVERTER_DIR = 'tflite_converter'
ACUITY_CONVERTER_DIR = 'acuity-tflite'

class ModelInstance:
    def __init__(self, model, package):
        self.model   = model
        self.package = package


class ModelSet:
    models = {
        # 'resnet101'         : ModelInstance(applications.ResNet101,         applications.resnet),
        # 'resnet152'         : ModelInstance(applications.ResNet152,         applications.resnet),
        # 'resnet50V2'        : ModelInstance(applications.ResNet50V2,        applications.resnet_v2),
        # 'resnet101V2'       : ModelInstance(applications.ResNet101V2,       applications.resnet_v2),
        # 'resnet152V2'       : ModelInstance(applications.ResNet152V2,       applications.resnet_v2),
        'mobilenetV2'       : ModelInstance(applications.MobileNetV2,       applications.mobilenet_v2),
    }

    def __init__(self):
        pass

    def _is_model(self, name):
        if name not in self.models:
            raise RuntimeError('Model {} does not exist'.format(name))

    def gen_path(self, name):
        self._is_model(name)
        return os.path.join('models', name)

    def create_dir(self, name, tflite=False):
        self._is_model(name)
        base_folder = self.gen_path(name)
        tflite_folder = os.path.join(base_folder, TF_LITE_CONVERTER_DIR)
        if not os.path.exists(base_folder):
            os.makedirs(base_folder)
        if tflite and not os.path.exists(tflite_folder):
            os.makedirs(tflite_folder)

    def gen_fn_keras(self, name):
        return name + '.h5'

    def gen_path_keras(self, name):
        return os.path.join(self.gen_path(name), self.gen_fn_keras(name))

    def gen_fn_tflite_float(self, name):
        return name + '.tflite'

    def get_path_tflite_quant(self, name, isFull):
        if isFull:
            return os.path.join(self.gen_path(name), TF_LITE_CONVERTER_DIR, name + 'quant-full.tflite')
        else:
            return os.path.join(self.gen_path(name), TF_LITE_CONVERTER_DIR, name + 'quant.tflite')

    def get_path_tflite_acuity(self, name):
        return os.path.join(self.gen_path(name), ACUITY_CONVERTER_DIR, name + '-uint8.tflite')