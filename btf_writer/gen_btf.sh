
self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
WORK_DIR="$self_dir/../deps"

ELF=$1
Pahole=${WORK_DIR}/dwarves-build/pahole
Write_elf=${self_dir}/../btf_writer/write_elf

gen_btf_elf() {
	$Pahole -J $ELF
	return
}

gen_sym_elf() {
	$Write_elf $ELF
	return
}

process_elf() {
	# elf=$1
	if [ ! -f $ELF ]; then
		echo "Error: $ELF is not exist!"
		exit -1
	fi

	# step-1: gen btf to elf
	gen_btf_elf
	# step-2: gen sym to elf
	# gen_sym_elf
}

process_elf $ELF