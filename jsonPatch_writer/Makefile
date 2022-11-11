CLANG ?= clang
CLANG_BPF_SYS_INCLUDES = $(shell $(CLANG) -v -E - </dev/null 2>&1 \
	| sed -n '/<...> search starts here:/,/End of search list./{ s| \(/.*\)|-idirafter \1|p }')

CFLAGS := -g -Wall -Wno-unused-function
LIBBPF_OBJ := libubpf.a

APP1=01-cve
APP2=02-cve
APP3=03-cve

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
all: $(APP1) $(APP2) $(APP3)

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

$(APP1): cJSON.o 01-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(APP2): cJSON.o 02-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

$(APP3): cJSON.o 03-cve.o $(LIBBPF_OBJ)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lm -o $@

.PHONY: compile
compile:
	$(call msg,GENERATE_EBPF_PATCH)
	$(Q)$(CLANG) -cc1 -fdump-record-layouts-simple $(CLANG_BPF_SYS_INCLUDES) -emit-llvm -D__TARGET_ARCH_x86_64 export_data_types.c > layout.txt
	$(Q)rm export_data_types.c export_data_types.ll

.PHONY: clean
clean:
	$(call msg,CLEAN, "0x-cve","*.patch.json")
	$(Q)rm -rf $(APP1) $(APP2) $(APP3) *.o *.patch.json