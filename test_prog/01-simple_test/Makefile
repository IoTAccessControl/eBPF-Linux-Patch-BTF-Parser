.PHONY: clean
all: target_simple

dir_bin:
	mkdir -p bin

target_libtest: | dir_bin
	gcc -g --shared -fPIC libtest.c -o bin/libtest.so

target_simple: target_libtest | dir_bin
	echo "build simple"
	gcc -g main.c -Ilibtest -Lbin -l:libtest.so -o bin/simple

clean:
	rm -rf bin
