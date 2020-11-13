import numpy as np

path = ['D:/jing.tang/TEST/SW/neural-network/unit_test/rpn_tests/rpn_001/res/float32/',
        'D:/jing.tang/TEST/SW/neural-network/unit_test/rpn_tests/rpn_001/res/float16/']

f1 = ['input_anchor32.bin','input_bbox32.bin',
      'input_img_info32.bin','input_score_before32.bin',
      'output_roi32.bin','output_score32.bin']
f2 = ['input_anchor16.bin','input_bbox16.bin',
      'input_img_info16.bin','input_score_before16.bin',
      'output_roi16.bin','output_score16.bin']

for i in range(len(f1)):
    n1 = np.fromfile(path[0] + f1[i], dtype=np.float32)
    n1 = n1.astype(np.float16)
    n1.tofile(path[1] + f2[i])