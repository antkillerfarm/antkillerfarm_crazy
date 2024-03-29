import os
import fnmatch

def iterfindfiles(path, fnexp):
    for root, dirs, files in os.walk(path):
        for filename in fnmatch.filter(files, fnexp):
            yield filename


def find_files_by_pattern(pattern, path='.'):
    paths = []
    for filename in iterfindfiles(path, pattern):
        if path not in paths:
            paths.append(filename)
    paths.sort()
    return paths

root_dir = '/home/1'
text_file = '/home/2'

with open(text_file) as fhndl:
    content = fhndl.read()

for path in find_files_by_pattern('*.c', path=root_dir):
    if content.find(path) == -1:
        print(path)