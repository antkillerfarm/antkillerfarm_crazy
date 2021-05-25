import os
import functools
from shutil import copyfile

def iterfindfiles(path, fnexp):
    for root, dirs, files in os.walk(path):
        for filename in files:
            if filename.endswith(fnexp):
                yield filename

def fitness(item):
    return int(item[4:-6])

def compare(item1, item2):
    return fitness(item1) - fitness(item2)

def find_files_by_pattern(pattern, path='.'):
    paths = []
    for filename in iterfindfiles(path, pattern):
        paths.append(filename)
    paths = sorted(paths, key=functools.cmp_to_key(compare))
    return paths

in_dir = '/home/ubuser/my/work/LBM_Taichi/out'
out_dir = '/home/ubuser/my/work/LBM_Taichi/out1'

num = 0
for path in find_files_by_pattern(('.png'), path=in_dir):
    path0 = int(path[4:-6])
    print(path0)
    copyfile(in_dir + '/' + path, out_dir + '/{:0>3d}.png'.format(num))
    num += 1
