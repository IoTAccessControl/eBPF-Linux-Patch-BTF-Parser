CLANG ?= clang
CLANG_BPF_SYS_INCLUDES = $(shell $(CLANG) -v -E - </dev/null 2>&1 \
	| sed -n '/<...> search starts here:/,/End of search list./{ s| \(/.*\)|-idirafter \1|p }')

CFLAGS := -g -Wall -Wno-unused-function
PYTHON := python3
LIBBPF_OBJ := libubpf.a

BIN_DIR = bin

MAIN_TEST=main_test
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
all: $(BIN_DIR)/$(MAIN_TEST) $(BIN_DIR)/$(APP1) $(BIN_DIR)/$(APP2) $(BIN_DIR)/$(APP3) $(BIN_DIR)/$(APP4) $(BIN_DIR)/$(APP5)

cJSON.o: cJSON.c
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) -c $^ -o $@

main_test.o: main_test.c
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

$(BIN_DIR)/$(MAIN_TEST): cJSON.o main_test.o $(LIBBPF_OBJ) | $(BIN_DIR)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(BIN_DIR)/$(APP1): cJSON.o 01-cve.o $(LIBBPF_OBJ) | $(BIN_DIR)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(BIN_DIR)/$(APP2): cJSON.o 02-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(BIN_DIR)/$(APP3): cJSON.o 03-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(BIN_DIR)/$(APP4): cJSON.o 04-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(BIN_DIR)/$(APP5): cJSON.o 05-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(BIN_DIR):
	mkdir $@

.PHONY: main_test
main_test:
	$(call msg,MAIN_TEST)
	$(Q)make all
	$(Q)rm *.o
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/01-cve -f orig_c0 -s 01-cve.bpf.c -o $(BIN_DIR)/01-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/02-cve -f orig_c0 -s 02-cve.bpf.c -o $(BIN_DIR)/02-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/03-cve -f orig_c0 -s 03-cve.bpf.c -o $(BIN_DIR)/03-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/04-cve -f orig_c0 -s 04-cve.bpf.c -o $(BIN_DIR)/04-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/05-cve -f orig_c0 -s 05-cve.bpf.c -o $(BIN_DIR)/05-cve.patch.json
	$(Q)$(BIN_DIR)/main_test

.PHONY: test
test:
	$(call msg,TEST_ALL)
	$(Q)make all
	$(Q)rm *.o
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/01-cve -f orig_c0 -s 01-cve.bpf.c -o $(BIN_DIR)/01-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/02-cve -f orig_c0 -s 02-cve.bpf.c -o $(BIN_DIR)/02-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/03-cve -f orig_c0 -s 03-cve.bpf.c -o $(BIN_DIR)/03-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/04-cve -f orig_c0 -s 04-cve.bpf.c -o $(BIN_DIR)/04-cve.patch.json
	$(Q)$(PYTHON) gen_patch.py -e $(BIN_DIR)/05-cve -f orig_c0 -s 05-cve.bpf.c -o $(BIN_DIR)/05-cve.patch.json
	$(Q)$(BIN_DIR)/01-cve
	$(Q)$(BIN_DIR)/02-cve
	$(Q)$(BIN_DIR)/03-cve
	$(Q)$(BIN_DIR)/04-cve
	$(Q)$(BIN_DIR)/05-cve

# .PHONY: compile
# compile:
# 	$(call msg,GENERATE_EBPF_PATCH)
# 	$(Q)$(CLANG) -cc1 -fdump-record-layouts-simple $(CLANG_BPF_SYS_INCLUDES) -emit-llvm -D__TARGET_ARCH_x86_64 export_data_types.c > layout.txt
# 	$(Q)rm export_data_types.c export_data_types.ll

.PHONY: clean
clean:
	$(call msg,CLEAN,"0x-cve","patch.json")
	$(Q)rm -rf $(BIN_DIR)/*.patch.json $(BIN_DIR)/*-cve $(BIN_DIR)/main_test *.o