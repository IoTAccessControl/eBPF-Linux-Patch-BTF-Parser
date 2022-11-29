#!/bin/python
# generate the json-style patch

import os
import re
import sys
import json
import argparse
import subprocess
from elftools.elf.elffile import ELFFile


CODEO = "code.o"
OUTPUT = "patch.json"
CONFIG_FILE = "config.json"
LAYOUT_OUTPUT_PATH = "layout.txt"
global_data = {}

def compile_code(src):
    cmd = f"clang -O2 -emit-llvm -c {src} -o - | llc -march=bpf -filetype=obj -o {CODEO}"
    # cmd = f"clang -O2 -target bpf -c {src} -o {CODEO} "
    exec_command(cmd, os.getcwd())
    with open(CODEO, "rb") as fp:
        elf = ELFFile(fp)
        code = elf.get_section_by_name('.text')
        ops = code.data()
        uc_str = "".join('\\x{:02x}'.format(c) for c in ops)
        global_data["ebpf_patch"] = uc_str
        global_data["ebpf_patch_len"] = len(ops) + 1
        global_data["name"] = src
    os.remove(CODEO)


def exec_command(cmd, cwd=os.getcwd()):
    # print(f"Run cmd '{cmd}' in '{cwd}'")
    try:
        result = subprocess.run(cmd, cwd=cwd, shell=True) # capture_output=True
        if result.returncode != 0:
            msg = f"returncode: {result.returncode} cmd: '{result.args}' err:{result.stderr}"
            print("ERROR", msg)
            return False
    except Exception as ex:
        import traceback
        traceback.print_exc()
        return False
    return True


def getVulFuncAddress(vul, name):
    with open(vul, "rb") as fp:
        elf = ELFFile(fp)
        code = elf.get_section_by_name('.symtab')
        if code.get_symbol_by_name(name):
            global_data["address_vulFunc"] = hex(code.get_symbol_by_name(name)[0].entry.st_value)
        if code.get_symbol_by_name("patch_handler"):
            global_data["address_patchHandler"] = hex(code.get_symbol_by_name("patch_handler")[0].entry.st_value)


def getVulPid(vul):
    with os.popen("ps x | grep " + vul + " | grep -v grep | awk '{print $1}'") as f:
        # print(int(f.readlines()[0]))
        pid = int(f.readlines()[0])
        global_data["pid"] = pid


def get_struct_memory_layout(f):
    json_data = {}
    json_data['Fields'] = []
    line_num = 2
    while True:
        line = f.readline()
        line_num = line_num + 1
        # print(line_num, line)
        if not line:
            break
        if line == "":
            continue
        if line_num == 3:  # Type: struct event
            find = re.search(r"Type: struct (.*)$", line)
            if not find:
                print("Error: line 3 struct")
                break
            # print("Struct Name: ", find.group(1))
            json_data['Struct Name'] = find.group(1)
        elif line_num == 6:  # Size:1472
            find = re.search(r"Size:(\d*)$", line)
            if not find:
                print("Error: line 6 Size")
                break
            # print("Size: ", find.group(1))
            json_data['Size'] = int(find.group(1))
        elif line_num == 7:  # DataSize:1472
            find = re.search(r"DataSize:(\d*)$", line)
            if not find:
                print("Error: line 7 DataSize")
                break
            # print("DataSize: ", find.group(1))
            json_data['DataSize'] = int(find.group(1))
        elif line_num == 8:  # Alignment:64
            find = re.search(r"Alignment:(\d*)$", line)
            if not find:
                print("Error: line 8 Alignment")
                break
            # print("Alignment: ", find.group(1))
            json_data['Alignment'] = int(find.group(1))
        # FieldOffsets: [0, 32, 64, 96, 128, 192, 256, 384, 1408]
        elif line_num == 9:
            find = re.search(r"FieldOffsets: (.*)>", line)
            if not find:
                print("Error: line 9 FieldOffsets")
                break
            # print("FieldOffsets: ", find.group(1))
            json_data['FieldOffsets'] = json.loads(find.group(1))
        #   LLVMType:%struct.event = type { i32, i32, i32, i32, i64, i64, [16 x i8], [127 x i8], i32 }
        elif line.startswith("  LLVMType"):
            find = re.search(r"  LLVMType:.* = type \{ (.*) \}", line)
            if not find:
                print("Error: LLVMType")
                break
            # print("LLVMType: ", find.group(1))
            LLVMType = find.group(1).split(', ')
            # TODO只是一个针对特殊情况的解决方案，不具有通用性，需要修改;获取两侧index?
            for index, item in enumerate(LLVMType):
                if "(" in item and ")" not in item:
                    LLVMType[index] = LLVMType[index] + ", " + LLVMType[index + 1]
                    del(LLVMType[index + 1])
            json_data['LLVMType'] = LLVMType
        elif line.startswith("*** Dumping AST Record Layout"):
            break
        # FieldDecl 0x1af20c8 <line:12:2, col:6> col:6 pid 'int'
        else:
            find = re.search(r"FieldDecl .* <.*> col:\d+ (.*) '(.*)'", line)
            if not find:
                continue
            # print("FieldDecl: ", find.group(1), find.group(2))
            field_data = {}
            field_data['Name'], field_data['Type'] = find.group(
                1), find.group(2)
            json_data['Fields'].append(field_data)
    # skip No export data
    if json_data == {"Fields": []}:
        return None

    # merge the types of LLVMType, FieldOffset and FieldDecl
    assert(len(json_data['Fields']) == len(json_data['LLVMType']))
    assert(len(json_data['Fields']) == len(json_data['FieldOffsets']))
    for i in range(len(json_data['Fields'])):
        json_data['Fields'][i]['LLVMType'] = json_data['LLVMType'][i]
        json_data['Fields'][i]['FieldOffset'] = json_data['FieldOffsets'][i]
    json_data.pop('LLVMType')
    json_data.pop('FieldOffsets')
    return json_data


# get the mem layout output of clang to json
def get_event_mem_layout_json(src):
    all_data = []
    with open(src) as f:
        line = f.readline()
        line = f.readline()
        while True:
            data = get_struct_memory_layout(f)
            if (data == None):
                break
            all_data.append(data)

    global_data["export_data_types"] = all_data
    # print(json.dumps(all_data, indent=4))


def fix_use_of_export_data(src):
    event_header_path = src[:-1] + "h"
    output_event_c_path = "export_data_types.c"
    c_generate_prefix = f"""
    // do not use this file: auto generated
    #include <stddef.h>
    #include <stdint.h>
    #include <stdbool.h>
    #include "{event_header_path}"

    // make the compile not ignore event struct
    """
    event_header_content = ""
    with open(event_header_path) as f:
        event_header_content = f.read()

    with open(output_event_c_path, "w") as f:
        finds = re.findall("(struct\s+(\w+))", event_header_content)
        # print(finds)
        tmp = sorted(set(finds), key=finds.index)
        for find in tmp:
            c_generate_prefix = c_generate_prefix + find[0] + "* " + find[1] + " = NULL;"
        f.write(c_generate_prefix)


def gen_json():
    with open(CONFIG_FILE) as f:
        config = json.load(f)
        config.update(global_data)
    with open(OUTPUT, "w") as f:
        f.write(json.dumps(config, indent=4, separators=(',', ': '))) # , sort_keys=True


def setup_args():
    parser = argparse.ArgumentParser(prog="gen_patch", epilog="e.g. python3 gen_patch.py -e tmp -f func -s code.c -o patch.json")
    # parser.add_argument("-a", "--asm", metavar="asm", help="compile asm.")
    parser.add_argument("-e", "--elf", metavar="elf", help="set vulnerable elf.")
    parser.add_argument("-f", "--func", metavar="func", help="set vulnerable func.")
    parser.add_argument("-s", "--src", metavar="src", help="choose ebpf src.")
    parser.add_argument("-o", "--output", metavar="output", help="set output file.")
    args = parser.parse_args()
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(-1)
    return args


def main():
    args = setup_args()
    if args.output:
        global OUTPUT
        OUTPUT = args.output
    if args.src:
        compile_code(args.src)
    if args.elf and args.func:
        getVulFuncAddress(args.elf, args.func)
        getVulPid(args.elf)
    # elif args.asm:
    #     assemble_to_bytecode(args.asm)
    fix_use_of_export_data(args.src)
    res = os.system("make compile")
    if res != 0:
        print("compile failed!")
        exit(1)
    get_event_mem_layout_json(LAYOUT_OUTPUT_PATH)

    # update json file
    gen_json()


if __name__ == '__main__':
    main()