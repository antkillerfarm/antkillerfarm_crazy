import numpy as np

path = 'D:/my/work/xx2/none_quantized_conv1d/'

file_src = ['weight.tensor','bias.tensor']
file_bin_format = [np.float16, np.float32]
file_bin_format_size = [2, 4]
file_bin_name = 'export.data'

offset = 0

file_bin = open(path + file_bin_name,'wb')
for i in range(len(file_src)):
    n1 = np.loadtxt(path + file_src[i], dtype=file_bin_format[i])
    print('{}/{}'.format(offset, len(n1) * file_bin_format_size[i]))
    offset += len(n1) * file_bin_format_size[i]
    n1.tofile(path + file_src[i] + '.data')
    file_n1 = open(path + file_src[i] + '.data', 'rb')
    data = file_n1.read()
    file_bin.write(data)
    file_n1.close()

file_bin.close()

print('{}'.format(offset))