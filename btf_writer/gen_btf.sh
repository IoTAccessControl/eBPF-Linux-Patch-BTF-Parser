
ELF=$1

gen_btf_elf() {
	return
}

gen_sym_elf() {
	return
}

process_elf() {
	elf=$1
	if [ ! -f $elf ]; then
		echo "Error: $elf is not exist!"
		exit -1
	fi

	# step-1: gen btf to elf

	# step-2: gen sym to elf
	
}

process_elf $ELF