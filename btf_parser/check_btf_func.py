#!/usr/bin/python
# -*- coding: UTF-8 -*-
import os
import sys


self_dir = os.getcwd()

def usage():
    print("Usage: python check.py filename, make sure the file has btf info\n")


def clean():
    os.system("rm ./tmp ./tmp2 ./funclist ./funclist_2")


def gen_diff(file_path):
    try:
        os.system(self_dir + "/btf_parser/build/parse_btf " + file_path + " all" + " | sort -t \":\" -k 2 > funclist")
    except IOError:
        print("Error: read BTF info failed\n")

    try:
        os.system("nm -s " + file_path + " | sort -t \" \" -k 2 > funclist_2")
    except IOError:
        print("Error: read symbols info failed\n")

    f = open("./funclist")
    line = f.readline()
    with open("./tmp", "w") as ftmp:
        while line:
            ftmp.write(line.split(" ")[1].lstrip())
            line = f.readline()

    f2 = open("./funclist_2")
    line2 = f2.readline()
    with open("tmp2", "w") as ftmp2:
        while line2:
            split_line2 = line2.split(" ")
            if split_line2[1] in ["t", "T"]:
                ftmp2.write(split_line2[2])
            line2 = f2.readline()

    os.system("diff -y ./tmp ./tmp2 > ./func_diff")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        usage()
        sys.exit(-1)
    gen_diff(sys.argv[1])
    clean()
