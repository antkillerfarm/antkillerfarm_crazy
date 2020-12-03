import numpy as np

#n1 = np.random.rand(65536)
#np.savetxt("input.tensor", n1, fmt='%f')

shape = (1, 33, 33, 21)
sum = 1
for i in range(len(shape)):
    sum = sum * shape[i]

n2 = np.random.rand(sum)
n2 = n2.reshape(shape)
file = 'input'
for index in range(len(n2.shape)):
    file += '_{}'.format(n2.shape[index])
np.save(file + ".npy", n2)
np.savetxt(file + ".tensor", n2.flatten(), fmt='%f')
'''
file_name = 'input_1_512_20'
n2 = np.load(file_name + '.npy')
n2 = n2.flatten()
np.savetxt(file_name + ".tensor", n2, fmt='%f')
'''
