import os
import numpy as np


def npy2text():
    src_path = "/home/ubuser/my/work/onnx_models/onnx_q_ops_test/q_linear_conv/"
    for src_file in os.listdir(src_path):
        if src_file.find(".npy") != -1:
            dst_file = src_file.replace(".npy", ".txt")
            print(src_file + " => " + dst_file)
            n1 = np.load(src_path + src_file)
            np.savetxt(src_path + dst_file, n1.flatten(), fmt='%.6f')


if __name__ == '__main__':
    npy2text()
