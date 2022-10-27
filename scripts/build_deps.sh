self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
WORK_DIR="$self_dir/../deps"

build_btfparser() {
	cd ${WORK_DIR}
	if [ ! -d ${WORK_DIR}/btfparse ]; then
		echo -e "\033[34mDownload btfparse...\033[0m"
		git clone --recursive https://github.com/trailofbits/btfparse.git
		cd btfparse
		git checkout tags/v1.1 -b tag-v1.1
		cd ..
	fi
	echo -e "\033[34mBuild btfparser to btfparse-build\033[0m"
	rm -rf btfparse-build
	cmake \
		-S btfparse \
		-B btfparse-build \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DBTFPARSE_ENABLE_TOOLS=true \
		-DBTFPARSE_ENABLE_TESTS=true
	cmake --build btfparse-build  -j $(nproc)
	return
}

build_pahole() {
	cd ${WORK_DIR}
	if [ ! -d ${WORK_DIR}/dwarves ]; then
		echo -e "\033[34mDownload dwarves...\033[0m"
		git clone --recursive https://github.com/acmel/dwarves.git
		cd dwarves
		git checkout tags/v1.22 -b tag-v1.22
		cd ..
		# 应用函数匿名化patch
		# cd dwarves/lib/bpf/
		# git apply ${self_dir}/../pahole_patch/pahole.patch
		# cd -
	fi
	echo -e "\033[34mBuild dwarves to dwarves-build\033[0m"
	rm -rf dwarves-build
	cmake \
		-S dwarves \
		-B dwarves-build \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-D__LIB=lib ..
	cmake --build dwarves-build  -j $(nproc)
	return
}

build_deps() {
	mkdir -p $WORK_DIR
	build_pahole
	build_btfparser
}

build_deps