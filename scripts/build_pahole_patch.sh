self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
WORK_DIR="$self_dir/../deps"

# Step-1: generate patch file for modifications in dwarves/lib/bpf/
cd ${WORK_DIR}/dwarves/lib/bpf/
git diff > pahole.patch
git format-patch pahole.patch
mv pahole.patch ${self_dir}/../patch/pahole.patch

# Step-2: rebuild dwarves
echo -e "\033[34mBuild dwarves to dwarves-build\033[0m"
cd ${WORK_DIR}
rm -rf dwarves-build
cmake \
	-S dwarves \
	-B dwarves-build \
	-DCMAKE_BUILD_TYPE=RelWithDebInfo \
	-D__LIB=lib ..

# apply patch to dwarves/lib/bpf/
cd ${WORK_DIR}/dwarves/lib/bpf/
git reset --hard HEAD@{0}
git apply ${self_dir}/../patch/pahole.patch
cd -

cmake --build dwarves-build  -j $(nproc)