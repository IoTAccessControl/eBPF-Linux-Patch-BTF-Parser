# Step-1: gen-btf
pushd ${ROOT_DIR}/test_prog/01-simple_test
make clean
make
echo "Gen BTF via pahole"
${ROOT_DIR}/deps/dwarves-build/pahole -J ${ROOT_DIR}/test_prog/01-simple_test/bin/simple
${ROOT_DIR}/deps/dwarves-build/pahole -J ${ROOT_DIR}/test_prog/01-simple_test/bin/libtest.so

# Step-2: add symbols to ELF  
popd