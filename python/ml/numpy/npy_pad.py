import numpy as np
arr1D = np.array([1, 1, 2, 2, 3, 4])
'''不同的填充方法'''
print('constant:  ' + str(np.pad(arr1D, (2, 3), 'constant')))
print('edge:  ' + str(np.pad(arr1D, (2, 3), 'edge')))
print('linear_ramp:  ' + str(np.pad(arr1D, (2, 3), 'linear_ramp')))
print('maximum:  ' + str(np.pad(arr1D, (2, 3), 'maximum')))
print('mean:  ' + str(np.pad(arr1D, (2, 3), 'mean')))
print('median:  ' + str(np.pad(arr1D, (2, 3), 'median')))
print('minimum:  ' + str(np.pad(arr1D, (2, 3), 'minimum')))
print('reflect:  ' + str(np.pad(arr1D, (2, 3), 'reflect')))
print('symmetric:  ' + str(np.pad(arr1D, (2, 3), 'symmetric')))
print('wrap:  ' + str(np.pad(arr1D, (2, 3), 'wrap')))
