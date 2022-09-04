self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
WORK_DIR="$self_dir/../deps"


build_btf_parser() {

	return
}

build_btf_writer() {

	return
}

build_all() {
	if [ ! -f ${WORK_DIR}/btfparse-build/btfparse/btfparse/libbtfparse.a ]; then
		echo "build deps"
		bash $self_dir/build_deps.sh
	fi

	build_btf_parser
	build_btf_writer
}

build_all