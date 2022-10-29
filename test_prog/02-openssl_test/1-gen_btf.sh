self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
root_dir=${self_dir}/../..

pushd ${self_dir}

if [[ ! -f openssl-3.0.5/libcrypto.so.3 ]]; then
	echo "build openssl first"
	bash build.sh
fi

rm -rf bin/ bin_btf/

mkdir -p bin
cp openssl-3.0.5/libcrypto.so.3 bin/
cp -r bin/ bin_btf/

# step 1: gen BTF
${root_dir}/deps/dwarves-build/pahole -J ${self_dir}/bin_btf/libcrypto.so.3
mv func_name_hash.txt bin_btf/func_name_hash.txt
${root_dir}/btf_parser/dump_btf ${self_dir}/bin_btf/libcrypto.so.3 > ${self_dir}/bin_btf/libcrypto.so.3_btf.txt

# step 2: strip
strip ${self_dir}/bin/libcrypto.so.3
strip ${self_dir}/bin_btf/libcrypto.so.3

echo "btf size increasing"
python3 ${root_dir}/btf_writer/check_size.py -c ${self_dir}/bin/libcrypto.so.3 ${self_dir}/bin_btf/libcrypto.so.3

echo "increase size after compressed"
tar -zcvf bin_btf/libcrypto.so.3.tar.gz bin_btf/libcrypto.so.3
python3 ${root_dir}/btf_writer/check_size.py -a ${self_dir}/bin/libcrypto.so.3 ${self_dir}/bin_btf/libcrypto.so.3.tar.gz