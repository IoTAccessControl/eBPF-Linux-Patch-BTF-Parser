self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
root_dir=${self_dir}/../..

echo "step-1: get btf info and save to dir: ${self_dir}/bin_btf/*.json"
bpftool -j btf dump file --pretty ${self_dir}/bin_btf/nginx > ${self_dir}/bin_btf/nginx.json 
${root_dir}/btf_parser/dump_btf ${self_dir}/bin_btf/nginx > ${self_dir}/bin_btf/nginx_btf.txt