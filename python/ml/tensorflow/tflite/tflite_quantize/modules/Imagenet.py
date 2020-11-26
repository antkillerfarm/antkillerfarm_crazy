
import xml.etree.ElementTree as ET
import json
import numpy as np
import os
from tensorflow.keras import preprocessing
from tensorflow import keras
import tqdm
import pandas as pd


dataset_path = "/home/ubuser/my/work/ILSVRC2015_val"
CLASS_INDEX_PATH = ('https://storage.googleapis.com/download.tensorflow.org/'
                    'data/imagenet_class_index.json')
DATASET_LOC= "/home/mfc/2019_AITEC/datasets/ILSVRC/Data/CLS-LOC/val/*"
#LABELS_FILE = "/home/mfc/2019_AITEC/datasets/ILSVRC/Data/CLS-LOC/ImagenetLabels1000.csv"
LABELS_FILE = dataset_path + "/ILSVRC2015_val_full.csv"

#LABELS_PD= pd.read_csv(LABELS_FILE, names=["file", "label"])
#LABELS_DICT = { os.path.splitext(x)[0] : y  for (x, y) in zip(LABELS_PD["file"], LABELS_PD["label"]) }

class ImagenetGenerator:
    def __init__(self, preprocess_input, image_width, image_heigth, num_images = None,  dataset = '', ):
        #IMAGENET_DATASET_ROOT = '/home/mfc/2019_AITEC/datasets/ILSVRC/'
        IMAGENET_DATASET_ROOT = dataset_path
        self.preprocess_input = preprocess_input
        self.image_width = image_width
        self.image_height = image_heigth
        #self.dataset = os.path. join(IMAGENET_DATASET_ROOT, 'Data', dataset)
        #self.anotations = os.path.join(IMAGENET_DATASET_ROOT, 'Annotations', dataset)
        #self.dataset = os.path. join(IMAGENET_DATASET_ROOT, 'data', dataset)
        self.dataset = dataset_path
        self.num_images = num_images
        self.labels_pd = pd.read_csv(LABELS_FILE, names=["file", "label"])
        self.labels_dict = { os.path.splitext(x)[0] : y  for (x, y) in zip(self.labels_pd["file"], self.labels_pd["label"]) }


    def _preprocess_image(self, filename):
        """Generator preprocessor func. Converts to <0-255> unscaled BGR values in BGR.

        Args:
            filename: path of the file being processed

        Returns:
            preprocessed image as a numpy array
        """
        img = preprocessing.image.load_img(filename, target_size=(self.image_width, self.image_height))
        x = preprocessing.image.img_to_array(img)
        x = np.expand_dims(x, axis=0)
        x = self.preprocess_input(x)
        return x

    def gen_fn(self):
        """Imagenet generator function. Generates an image sample (should be from imagenet2012).

        Returns:
            tuple(filename, image_data)
        """
        files = os.listdir(self.dataset)
        if(self.num_images is not None):
            files = files[0:self.num_images]
        for f in tqdm.tqdm(files):
            yield (os.path.basename(f), [self._preprocess_image(os.path.join(self.dataset, f))])

    def gen_repr_dataset(self):
        """
        files = os.listdir(self.dataset)
        if(self.num_images is not None):
            files = files[0:self.num_images]
        for f in tqdm.tqdm(files):
            yield [self._preprocess_image(os.path.join(self.dataset, f))]
        """
        files = self.labels_pd["file"].to_list()
        if(self.num_images is not None):
            files = files[0:self.num_images]
        for f in tqdm.tqdm(files):
            yield [self._preprocess_image(os.path.join(self.dataset, f))]

    def gen_validation_data(self):
        """Imagenet generator function for model validation
        Returns:
            tuple(image_data, label)
        """
        files = os.listdir(self.dataset)
        if (self.num_images is not None):
            files = files[0:self.num_images]
        for f in tqdm.tqdm(files):
            label = self.labels_dict[os.path.splitext(os.path.basename(f))[0]]
            yield ([self._preprocess_image(os.path.join(self.dataset, f))], label )

    def get_class(self, image_f_name):
        """anot_file = os.path.join(self.anotations,  os.path.splitext(image_f_name)[0] + '.xml')
        tree = ET.parse(anot_file)
        root = tree.getroot()
        category = root.findtext('./object/name')

        fpath = keras.utils.get_file(
            'imagenet_class_index.json',
            CLASS_INDEX_PATH,
            cache_subdir='models',
            file_hash='c2c37ea517e94d9795004a39431a14cb')
        """
        fpath = dataset_path + "/imagenet_class_index.json"
        with open(fpath) as f:
            CLASS_INDEX = json.load(f)
            class_index = {i[0] : i[1] for i in CLASS_INDEX.values()}

        return class_index[category]

    def get_class_index(self, image_f_name):
        """
        anot_file = os.path.join(self.anotations,  os.path.splitext(image_f_name)[0] + '.xml')
        tree = ET.parse(anot_file)
        root = tree.getroot()
        category = root.findtext('./object/name')

        fpath = keras.utils.get_file(
            'imagenet_class_index.json',
            CLASS_INDEX_PATH,
            cache_subdir='models',
            file_hash='c2c37ea517e94d9795004a39431a14cb')
        """
        fpath = dataset_path + "/imagenet_class_index.json"
        with open(fpath) as f:
            CLASS_INDEX = json.load(f)
            class_index = {i[0] : i[1] for i in CLASS_INDEX.values()}

        return class_index[category]
