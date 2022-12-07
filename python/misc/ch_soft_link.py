import os
import fnmatch

def iterfindfiles(path, fnexp):
    for root, dirs, files in os.walk(path):
        for filename in fnmatch.filter(files, fnexp):
            yield os.path.join(root, filename)


def find_files_by_pattern(pattern, path='.'):
    paths = []
    for filename in iterfindfiles(path, pattern):
        if path not in paths:
            paths.append(filename)
    paths.sort()
    return paths

root_dir = "/home"
p1 = "/home/work/tensorflow"
p2 = "/home/work/xla/tensorflow"

for path in find_files_by_pattern('BUILD.bazel', path=root_dir):
    if (os.path.islink(path)):
        link_path = os.readlink(path)
        if (link_path.count(p1) > 0):
            link_path = link_path.replace(p1, p2)
            os.remove(path)
            os.symlink(link_path, path)
