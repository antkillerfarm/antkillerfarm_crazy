#!/usr/bin/env python3
import os
import pandas as pd
import tqdm

dataset_path = "/home/ubuser/my/work/ILSVRC2015_val"
LABELS_FILE = dataset_path + "/ILSVRC2015_val.csv"
LABELS_FULL_FILE = dataset_path + "/ILSVRC2015_val_full.csv"
FILELIST = dataset_path + "/filelist.csv"

labels_pd = pd.read_csv(LABELS_FILE, names=["file", "label"])
file_pd = pd.read_csv(FILELIST, names=["file"])
labels_full_pd = pd.DataFrame()

for (x, y) in tqdm.tqdm(zip(labels_pd["file"], labels_pd["label"])):
    for x1 in file_pd["file"]:
        if x in x1:
            labels_full_pd = labels_full_pd.append({'file' : x1, 'label' : str(y)}, ignore_index=True)

labels_full_pd.to_csv(LABELS_FULL_FILE, sep=",", index=False, header=False)
