# /usr/bin/python3

import os
import sys

def get_file_size(fi) -> int:
	if not os.path.isfile(fi):
		raise Exception(f"{fi} is not a file")
	file_stats = os.stat(fi)
	return file_stats.st_size

def get_kb(num) -> float:
	return round(num / 1024, 2)

def compare_two(f1, f2):
	f1_sz = get_file_size(f1)
	print(f"file: {f1} size: {f1_sz}")
	f2_sz = get_file_size(f2)
	print(f"file: {f2} size: {f2_sz}")
	if f1_sz > f2_sz:
		tmp = f1_sz
		f1_sz = f2_sz
		f2_sz = tmp
	inc_size = f2_sz - f1_sz
	precent = round(inc_size * 100 / f1_sz, 1)
	kb_inc = get_kb(inc_size)
	f1_kb = get_kb(f1_sz)
	print(f"\tincrease: {f1_sz} -> {f2_sz} (inc: {inc_size} {f1_kb}kb + {kb_inc}kb {precent}%)")

def calc_inc(f1, f1_add):
	f1_sz = get_file_size(f1)
	f1_add_sz = get_file_size(f1_add)
	f1_kb = get_kb(f1_sz)
	f1_add_kb = get_kb(f1_add_sz)
	precent = round(f1_add_sz * 100 / f1_sz, 1)
	print(f"\tincrease: {f1_sz} + {f1_add_sz} ({f1_kb}kb + {f1_add_kb}kb {precent}%)")


def usage():
	print("Usage: python3 check_size.py [-c|-a] f1 f2")
	print("\t-c: compare the increase bytes of two files (f1, f2)")
	print("\t-a: calculate increase of f1 with (f1 + f2)")

def main():
	if len(sys.argv) < 3:
		usage()
		exit(-1)
	cmd = sys.argv[1]
	f1 = sys.argv[2]
	f2 = sys.argv[3]
	if cmd == "-c":
		compare_two(f1, f2)
	elif cmd == "-a":
		calc_inc(f1, f2)
	else:
		usage()

if __name__ == "__main__":
	main()
