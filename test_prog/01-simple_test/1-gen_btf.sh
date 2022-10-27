self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
root_dir=${self_dir}/../..

# build
pushd ${self_dir}
make clean
make
echo "Gen BTF via pahole"
# ls -la bin/
cp -r bin/ bin_btf/
# step-1: add BTF
${root_dir}/deps/dwarves-build/pahole -J ${root_dir}/test_prog/01-simple_test/bin_btf/simple
# ${root_dir}/deps/dwarves-build/pahole -J ${root_dir}/test_prog/01-simple_test/bin_btf/libtest.so
# step-2: add func addresses
# ${ROOT_DIR}/btf_writer/write_elf ${ROOT_DIR}/test_prog/01-simple_test/bin/simple
# ${ROOT_DIR}/btf_writer/gen_btf.sh ${ROOT_DIR}/test_prog/01-simple_test/bin_btf/simple
${ROOT_DIR}/btf_writer/gen_btf.sh ${ROOT_DIR}/test_prog/01-simple_test/bin_btf/libtest.so

popd