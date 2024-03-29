self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
root_dir=${self_dir}/../..

pushd ${self_dir}

if [[ ! -f httpd-2.4.54/httpd ]]; then
	echo "build httpd first"
	bash build.sh
fi

rm -rf bin/ bin_btf/

mkdir -p bin
cp httpd-2.4.54/httpd bin/
cp -r bin/ bin_btf/

# step 1: gen BTF
${root_dir}/deps/dwarves-build/pahole -J ${self_dir}/bin_btf/httpd
mv func_name_hash.txt bin_btf/func_name_hash.txt
${root_dir}/btf_parser/dump_btf ${self_dir}/bin_btf/httpd > ${self_dir}/bin_btf/httpd_btf.txt

# step 2: strip
strip ${self_dir}/bin/httpd
strip ${self_dir}/bin_btf/httpd

echo "btf size increasing"
python3 ${root_dir}/btf_writer/check_size.py -c ${self_dir}/bin/httpd ${self_dir}/bin_btf/httpd

echo "increase size after compressed"
echo "via tar"
tar -zcvf bin_btf/httpd.tar.gz bin_btf/httpd.btf
python3 ${root_dir}/btf_writer/check_size.py -a ${self_dir}/bin/httpd ${self_dir}/bin_btf/httpd.tar.gz
echo "via zip"
zip bin_btf/httpd.zip bin_btf/httpd.btf
python3 ${root_dir}/btf_writer/check_size.py -a ${self_dir}/bin/httpd ${self_dir}/bin_btf/httpd.zip