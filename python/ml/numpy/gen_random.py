import numpy as np

def print_np(n):
    for index in range(len(n)):
        print("{}, ".format(n[index]), end='')
        if (index % 5 == 4):
            print("")

n2 = np.random.rand(120000)
print("random:")
print_np(n2)
n3 = np.sort(n2)
n3 = n3[::-1]
print("sort:")
print_np(n3)
