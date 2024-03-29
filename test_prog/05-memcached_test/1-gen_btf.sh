self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
root_dir=${self_dir}/../..

pushd ${self_dir}

if [[ ! -f memcached-1.6.17/memcached ]]; then
	echo "build memcached first"
	bash build.sh
fi

rm -rf bin/ bin_btf/

mkdir -p bin
cp memcached-1.6.17/memcached bin/
cp -r bin/ bin_btf/

# step 1: gen BTF
${root_dir}/deps/dwarves-build/pahole -J ${self_dir}/bin_btf/memcached
mv func_name_hash.txt bin_btf/func_name_hash.txt
${root_dir}/btf_parser/dump_btf ${self_dir}/bin_btf/memcached > ${self_dir}/bin_btf/memcached_btf.txt

# step 2: strip
strip ${self_dir}/bin/memcached
strip ${self_dir}/bin_btf/memcached

echo "btf size increasing"
python3 ${root_dir}/btf_writer/check_size.py -c ${self_dir}/bin/memcached ${self_dir}/bin_btf/memcached

echo "increase size after compressed"
echo "via tar"
tar -zcvf bin_btf/memcached.tar.gz bin_btf/memcached.btf
python3 ${root_dir}/btf_writer/check_size.py -a ${self_dir}/bin/memcached ${self_dir}/bin_btf/memcached.tar.gz
echo "via zip"
tar -zcvf bin_btf/memcached.zip bin_btf/memcached.btf
python3 ${root_dir}/btf_writer/check_size.py -a ${self_dir}/bin/memcached ${self_dir}/bin_btf/memcached.zip