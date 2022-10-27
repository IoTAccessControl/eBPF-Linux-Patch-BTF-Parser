self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
root_dir=${self_dir}/../..

pushd ${self_dir}

if [[ ! -f nginx-1.22.1/objs/nginx ]]; then
	echo "build nginx first"
	bash build.sh
fi

rm -rf bin/ bin_btf/

mkdir -p bin
cp nginx-1.22.1/objs/nginx bin/
cp -r bin/ bin_btf/

# step 1: gen BTF
${root_dir}/deps/dwarves-build/pahole -J ${self_dir}/bin_btf/nginx
mv func_name_hash.txt bin_btf/func_name_hash.txt

# step 2: strip
strip ${self_dir}/bin/nginx
strip ${self_dir}/bin_btf/nginx

echo "btf size increasing"
python3 ${root_dir}/btf_writer/check_size.py ${self_dir}/bin/nginx ${self_dir}/bin_btf/nginx
