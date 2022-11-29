CLANG ?= clang
CLANG_BPF_SYS_INCLUDES = $(shell $(CLANG) -v -E - </dev/null 2>&1 \
	| sed -n '/<...> search starts here:/,/End of search list./{ s| \(/.*\)|-idirafter \1|p }')

CFLAGS := -g -Wall -Wno-unused-function
PYTHON := python3
LIBBPF_OBJ := libubpf.a

APP1=01-cve
APP2=02-cve
APP3=03-cve
APP4=04-cve
APP5=05-cve

ifeq ($(V),1)
	Q =
	msg =
else
	Q = @
	msg = @printf '  %-8s %s%s\n'					\
			  "$(1)"						\
			  "$(patsubst $(abspath $(OUTPUT))/%,%,$(2))"	\
			  "$(if $(3), $(3))";
	MAKEFLAGS += --no-print-directory
endif

.PHONY: all
all: $(APP1) $(APP2) $(APP3) $(APP4) $(APP5)

cJSON.o: cJSON.c
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) -c $^ -o $@

01-cve.o: 01-cve.c
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) -c $^ -o $@

02-cve.o: 02-cve.c
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) -c $^ -o $@

03-cve.o: 03-cve.c
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) -c $^ -o $@

04-cve.o: 04-cve.c
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) -c $^ -o $@

05-cve.o: 05-cve.c
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) -c $^ -o $@

$(APP1): cJSON.o 01-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(APP2): cJSON.o 02-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(APP3): cJSON.o 03-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(APP4): cJSON.o 04-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(APP5): cJSON.o 05-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

.PHONY: test
test:
	$(call msg,TEST_ALL)
	$(Q)make all
	$(Q)$(PYTHON) gen_patch.py -e 01-cve -f orig_c0 -s 01-cve.bpf.c -o 01-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e 02-cve -f orig_c0 -s 02-cve.bpf.c -o 02-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e 03-cve -f orig_c0 -s 03-cve.bpf.c -o 03-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e 04-cve -f orig_c0 -s 04-cve.bpf.c -o 04-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e 05-cve -f orig_c0 -s 05-cve.bpf.c -o 05-cve.patch.json
	$(Q)./01-cve
	$(Q)./02-cve
	$(Q)./03-cve
	$(Q)./04-cve
	$(Q)./05-cve

.PHONY: compile
compile:
	$(call msg,GENERATE_EBPF_PATCH)
	$(Q)$(CLANG) -cc1 -fdump-record-layouts-simple $(CLANG_BPF_SYS_INCLUDES) -emit-llvm -D__TARGET_ARCH_x86_64 export_data_types.c > layout.txt
	$(Q)rm export_data_types.c export_data_types.ll

.PHONY: clean
clean:
	$(call msg,CLEAN, "0x-cve","*.patch.json")
	$(Q)rm -rf $(APP1) $(APP2) $(APP3) $(APP4) $(APP5) *.o *.patch.json