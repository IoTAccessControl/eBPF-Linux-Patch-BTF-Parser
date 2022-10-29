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

# step-1: add BTF section
${root_dir}/deps/dwarves-build/pahole -J ${self_dir}/bin_btf/simple
mv func_name_hash.txt bin_btf/simple_func_name_hash.txt
${root_dir}/btf_parser/dump_btf ${self_dir}/bin_btf/simple > ${self_dir}/bin_btf/simple_btf.txt

${root_dir}/deps/dwarves-build/pahole -J ${self_dir}/bin_btf/libtest.so
mv func_name_hash.txt bin_btf/libtest_func_name_hash.txt
${root_dir}/btf_parser/dump_btf ${self_dir}/bin_btf/libtest.so > ${self_dir}/bin_btf/libtest.so_btf.txt

# step-2: add func addresses
# ${ROOT_DIR}/btf_writer/write_elf ${ROOT_DIR}/test_prog/01-simple_test/bin/simple
# ${ROOT_DIR}/btf_writer/gen_btf.sh ${ROOT_DIR}/test_prog/01-simple_test/bin_btf/simple
# ${ROOT_DIR}/btf_writer/gen_btf.sh ${ROOT_DIR}/test_prog/01-simple_test/bin_btf/libtest.so

# step-2: strip files and check size
strip ${self_dir}/bin/simple
strip ${self_dir}/bin/libtest.so
strip ${self_dir}/bin_btf/simple
strip ${self_dir}/bin_btf/libtest.so

echo "btf size increasing"
python3 ${root_dir}/btf_writer/check_size.py -c ${self_dir}/bin/simple ${self_dir}/bin_btf/simple
python3 ${root_dir}/btf_writer/check_size.py -c ${self_dir}/bin/libtest.so ${self_dir}/bin_btf/libtest.so

echo "increase size after compressed"
tar -zcvf bin_btf/simple.tar.gz bin_btf/simple
python3 ${root_dir}/btf_writer/check_size.py -a ${self_dir}/bin/simple ${self_dir}/bin_btf/simple.tar.gz
tar -zcvf bin_btf/libtest.so.tar.gz bin_btf/libtest.so
python3 ${root_dir}/btf_writer/check_size.py -a ${self_dir}/bin/libtest.so ${self_dir}/bin_btf/libtest.so.tar.gz

popd