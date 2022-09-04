
self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
WORK_DIR="$self_dir/../deps"

build_btfparser() {
	cd ${WORK_DIR}
	if [ ! -d ${WORK_DIR}/btfparse ]; then
		echo "download btfparse..."
		git clone --recursive https://github.com/trailofbits/btfparse.git
		cd btfparse
		git checkout tags/v1.1 -b tag-v1.1
		cd ..
	fi
	echo "build btfparser to btfparse-build"
	rm -rf btfparse-build
	cmake \
		-S btfparse \
		-B btfparse-build \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DBTFPARSE_ENABLE_TOOLS=true \
		-DBTFPARSE_ENABLE_TESTS=true
	cmake --build btfparse-build  -j $(nproc)
}

build_pahole() {
	# git apply 
	# git clone https://github.com/acmel/dwarves/tree/v1.24
	return
}

build_deps() {
	mkdir -p $WORK_DIR
	build_pahole
	build_btfparser
}

build_deps