import numpy as np

def find_first_big_error(n1, n2, error):
    for i in range(len(n1)):
        n = abs(n1[i]-n2[i])
        if n > error:
            return i

path = ['D:/my/fast/2017_10_24_14_10_42/', 'D:/my/fast/']

f1 = ['relu1_2_out0.tensor']
f2 = ['ConvolutionReluLayer_0_400_300_96_1.txt']

for i in range(len(f1)):
    print ("Loading Layer {}".format(i))
    n1 = np.loadtxt(path[0] + f1[i], dtype=np.float32)
    print("n1: Max: %f - %d Min: %f - %d" % (n1.max(), n1.argmax(), n1.min(), n1.argmin()))
    n2 = np.loadtxt(path[1] + f2[i], dtype=np.float32)
    print("n2: Max: %f - %d Min: %f - %d" % (n2.max(), n2.argmax(), n2.min(), n2.argmin()))
    if n1.shape[0] != n2.shape[0]:
        print("Shapes are not equal!")
    else:
        #num = find_first_big_error(n1, n2, 10)
        #print("num: {}".format(num))
        n3 = n1 - n2
        print("n3: Max: %f - %d Min: %f - %d" % (n3.max(), n3.argmax(), n3.min(), n3.argmin()))
        n4 = n3.std()
        print("STD: %f"% n4)

