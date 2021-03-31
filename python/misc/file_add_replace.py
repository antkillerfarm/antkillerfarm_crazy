import re
pattern = re.compile(r'\d+')

def get_blank_num(op, total_num):
    res = total_num - len(op)
    if res < 1:
        res = 1
    return res

def modify_file(modify_list, env):
    for m in modify_list:
        file = env.path + m['file']
        print("modify "+ file)
        with open(file, mode='r+', newline='\n', encoding='UTF-8') as fhndl:
            lines = m['func'](fhndl.readlines(), env)
            fhndl.seek(0)
            fhndl.writelines(lines)

def replace_keywords(op, new_lines, env):
    op = env.prefix + op
    KERNEL_OP_LOWER = op.lower()
    KERNEL_OP = op.upper()
    KERNEL_OP_STD = op[0].upper() + op[1:].lower()
    for index, line in enumerate(new_lines):
        if line.find('#KERNEL_OP#') != -1:
            new_lines[index] = new_lines[index].replace('#KERNEL_OP#', KERNEL_OP)
        if line.find('#KERNEL_OP_LOWER#') != -1:
            new_lines[index] = new_lines[index].replace('#KERNEL_OP_LOWER#', KERNEL_OP_LOWER)
        if line.find('#KERNEL_OP_STD#') != -1:
            new_lines[index] = new_lines[index].replace('#KERNEL_OP_STD#', KERNEL_OP_STD)
    return new_lines

def add_file(add_list, env):
    for a in add_list:
        file = env.path + a['file']
        with open(file, mode='r', newline='\n', encoding='UTF-8') as fhndl:
            lines = fhndl.readlines()
        for op in env.op_name:
            new_file = a['new_file'](op, env)
            print("add " + new_file)
            new_lines = lines.copy()
            with open(new_file, mode='w', newline='\n', encoding='UTF-8') as fhndl:
                new_lines = replace_keywords(op, new_lines, env)
                fhndl.writelines(new_lines)

def modify_BUILD(lines, env):
    new_lines = lines.copy()
    offset = 0
    for index, line in enumerate(lines):
        if line.find('include/client/vsi_nn_vxkernel.h') != -1:
            for op in env.op_name:
                new_lines.insert(index + offset, '        "include/ops/vsi_nn_op_' +
                             op.lower() + '.h",\n')
                offset += 1
        if line.find('src/libnnext/ops/kernel/vsi_nn_kernel_argmax.c') != -1:
            for op in env.op_name:
                new_lines.insert(index + offset, '        "src/ops/vsi_nn_op_' +
                             op.lower() + '.c",\n')
                offset += 1
        if (not env.without_kernel) and line.find('src/libnnext/vsi_nn_libnnext_vx.c') != -1:
            for op in env.op_name:
                new_lines.insert(index + offset, '        "src/libnnext/ops/kernel/vsi_nn_kernel_' +
                                 op.lower() + '.c",\n')
                offset += 1
    return new_lines

def proc_file(proc_list):
    for a in proc_list:
        file = a['file']
        with open(file, mode='r', newline='\n', encoding='UTF-8') as fhndl:
            lines = fhndl.readlines()
        new_file = a['new_file']
        print("proc " + file)
        new_lines = []
        offset = 0
        with open(new_file, mode='w', newline='\n', encoding='UTF-8') as fhndl:
            for _, line in enumerate(lines):
                if line.find('execution time:') != -1:
                    new_line = pattern.findall(line)
                    new_lines.insert(offset, new_line[0] + '\n')
                    offset += 1
            fhndl.writelines(new_lines)

proc_list = [
    {'file': '/nfs-server/ONNX/bin/1.txt',
     'new_file': '/nfs-server/ONNX/bin/1_a.txt'},
    {'file': '/nfs-server/ONNX/v1.1.2/1.txt',
     'new_file': '/nfs-server/ONNX/v1.1.2/1_a.txt'},
]

proc_file(proc_list)
