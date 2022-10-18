self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
WORK_DIR="$self_dir/../deps"


build_btf_parser() {
	# 调用btfparse库
	cd ${self_dir}/../btf_parser/
	rm -rf build/ btfparse/
	cp -r ${WORK_DIR}/btfparse/ .
	mkdir build && cd build
	cmake .. && make
	return
}

build_btf_writer() {
	# 生成write_elf文件
	cd ${self_dir}/../btf_writer/
	gcc -o write_elf write_elf.c
	return
}

build_all() {
	if [ ! -f ${WORK_DIR}/btfparse-build/btfparse/btfparse/libbtfparse.a ]; then
		echo -e "\033[34mBuild deps...\033[0m"
		bash $self_dir/build_deps.sh
	fi

	build_btf_parser
	build_btf_writer
}

build_all