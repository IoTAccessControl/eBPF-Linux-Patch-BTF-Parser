# /usr/bin/python3

import os
import sys

def get_file_size(fi) -> int:
	if not os.path.isfile(fi):
		raise Exception(f"{fi} is not a file")
	file_stats = os.stat(fi)
	return file_stats.st_size


def main():
	if len(sys.argv) < 2:
		print("Usage: python3 check_size.py f1 [f2]")
		exit(-1)
	f1 = sys.argv[1]
	f1_sz = get_file_size(f1)
	print(f"file: {f1} size: {f1_sz}")
	if len(sys.argv) > 2:
		f2 = sys.argv[2]
		f2_sz = get_file_size(f2)
		print(f"file: {f2} size: {f2_sz}")
		if f1_sz > f2_sz:
			tmp = f1_sz
			f1_sz = f2_sz
			f2_sz = tmp
		inc_size = f2_sz - f1_sz
		precent = round(inc_size * 100 / f1_sz, 1)
		print(f"\tincrease: {f1_sz} -> {f2_sz} ({inc_size} {precent}%)")

if __name__ == "__main__":
	main()
