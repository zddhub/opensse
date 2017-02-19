#!/usr/bin/python

import sys, os, getopt
import random

def usage():
    print "Usage: generate_filelist -d rootdir -f filter -o filelist [-n sample_num]"
    print "  rootdir:\t file root directory"
    print "  filter:\t file name filter, eg: \"*.png\" "
    print "  filelist:\t output file name"
    print "  sample_num:\t sample num (>0)"

def generate_filelist(argv):
    if len(argv) != 6 and len(argv) != 8:
        usage()
        sys.exit(1)

    rootdir = ''
    filter = ''
    filelist = ''
    sample_num = 0
    try:
        opts, args = getopt.getopt(argv, "d:f:o:n:", ["rootdir=", "filter=", "filelist=", "sample_num="])
    except getopt.GetoptError:
        usage()
        sys.exit(1)

    for opt, arg in opts:
        if opt in ("-d", "--rootdir"):
            rootdir = arg
        elif opt in ("-f", "--filter"):
            filter = arg
        elif opt in ("-o", "--output"):
            filelist = arg
        elif opt in ("-n", "--sample_num"):
            sample_num = int(arg)
            if sample_num < 0:
                usage()
                sys.exit(1)
        else:
            usage()
            sys.exit(1)

    lists = []
    os.chdir(rootdir)

    for root, dirs, files in os.walk('.'):
        for file in files:
            if file.endswith(filter[1:]):
                lists.append(os.path.join(root, file))

    list = range(0, len(lists))
    indexes = []
    if sample_num == 0:
        indexes = list
    else:
        indexes = random.sample(list, sample_num)

    with open(filelist, 'w') as f:
        for index in indexes:
            f.write(lists[index][2:] + '\n')


if __name__ == "__main__":
    generate_filelist(sys.argv[1:])

