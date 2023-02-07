import numpy as np

x = [1.0, 2.0, 3.0] 
a = np.asarray(x, dtype=np.float32)
txt = a.tobytes()
print(''.join(['%02X ' % b for b in txt]))
