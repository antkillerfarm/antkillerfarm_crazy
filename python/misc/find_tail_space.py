import sys
from argparse import ArgumentParser

def main(argv):
    options = ArgumentParser(description='find tail space')
    options.add_argument('files', nargs = '*')
    args = options.parse_args()
    for f in args.files:
        print("File: {}".format(f))
        with open(f, mode='r', newline='\n', encoding='UTF-8') as fhndl:
            lines = fhndl.readlines()
            for index, line in enumerate(lines):
                if line[len(line) - 2] == ' ':
                    print("Line: {}".format(index + 1))

if __name__ == '__main__':
    main(sys.argv)
