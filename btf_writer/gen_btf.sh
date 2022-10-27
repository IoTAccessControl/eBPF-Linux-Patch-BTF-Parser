
self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
WORK_DIR="$self_dir/../deps"

ELF=$1
Pahole=${WORK_DIR}/dwarves-build/pahole
Write_elf=${self_dir}/write_elf

# echo "self dir: ${self_dir}"

process_elf() {
	# elf=$1
	if [ ! -f $ELF ]; then
		echo "Error: $ELF is not exist!"
		exit -1
	fi

	# step-1: gen btf to elf
	$Pahole -J $ELF
	# step-2: gen sym to elf
	$Write_elf $ELF
}

process_elf $ELF