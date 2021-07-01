import numpy as np

def print_top5(input):
    k = 5
    n = np.partition(input.flatten(), -k)[-k:]
    n = np.sort(n)[::-1]
    for i in range(k):
        print("{} : {}".format(i, n[i]))

def print_top5_index(input):
    k = 5
    n = input.flatten()
    n_arg0 = np.argpartition(n, -k)[-k:]
    n = n[n_arg0]
    n_arg1 = np.argsort(n)[::-1]
    n_arg0 = n_arg0[n_arg1]
    for i in range(k):
        print("{} : {}".format(n_arg0[i], n[n_arg1[i]]))

x = np.array([3, 5, 6, 4, 2, 7, 1, 22, 33, -1, -8])
#print_top5(x)
print_top5_index(x)
