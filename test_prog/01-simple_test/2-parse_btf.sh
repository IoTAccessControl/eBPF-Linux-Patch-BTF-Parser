self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
root_dir=${self_dir}/../..

# step-1: ground truth
echo "step-1: get btf info and save to dir: ${self_dir}/bin_btf/*.json"
bpftool -j btf dump file --pretty ${self_dir}/bin_btf/simple > ${self_dir}/bin_btf/simple.json 
bpftool -j btf dump file --pretty ${self_dir}/bin_btf/libtest.so > ${self_dir}/bin_btf/libtest.json 
${root_dir}/btf_parser/dump_btf ${self_dir}/bin_btf/simple > ${self_dir}/bin_btf/simple_btf.txt 
${root_dir}/btf_parser/dump_btf ${self_dir}/bin_btf/libtest.so > ${self_dir}/bin_btf/libtest_btf.txt 

# step-2: use our parse tool to get all func
echo "step-2: get all functions and save to dir: ${self_dir}/bin_btf/*_all.txt"
${root_dir}/btf_parser/build/parse_btf ${self_dir}/bin_btf/simple all > ${self_dir}/bin_btf/simple_all.txt
${root_dir}/btf_parser/build/parse_btf ${self_dir}/bin_btf/libtest.so all > ${self_dir}/bin_btf/libtest_all.txt

# step-3: show detailed info of a function
echo "step-3: generate btf details for functions and save to dir: ${self_dir}/bin_btf/*_func.txt"
${root_dir}/btf_parser/build/parse_btf ${self_dir}/bin_btf/simple main single > ${self_dir}/bin_btf/simple_main_func.txt
${root_dir}/btf_parser/build/parse_btf ${self_dir}/bin_btf/libtest.so libtest_add single > ${self_dir}/bin_btf/libtest_add_func.txt

echo "Finish all steps..." 
echo "Please find the btf details in dir: ${self_dir}/bin_btf/"