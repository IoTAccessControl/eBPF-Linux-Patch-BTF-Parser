self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
root_dir=${self_dir}/../..

echo "step-1: get btf info and save to dir: ${self_dir}/bin_btf/*.json"
bpftool -j btf dump file --pretty ${self_dir}/bin_btf/nginx > ${self_dir}/bin_btf/nginx.json 

echo "step-2: get all functions and save to dir: ${self_dir}/bin_btf/*_all.txt"
${root_dir}/btf_parser/build/parse_btf ${self_dir}/bin_btf/nginx all > ${self_dir}/bin_btf/nginx_all.txt
