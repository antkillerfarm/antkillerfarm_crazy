import re
pattern = re.compile(r'= "?(stablehlo.\w*)')

file = "/home/ubuser/下载/profiler.txt"

def find_and_count():
    with open(file, mode='r', newline='\n', encoding='UTF-8') as fhndl:
        lines = fhndl.readlines()
        new_lines = []
        for _, line in enumerate(lines):
            result = pattern.findall(line)
            if len(result) > 0:
                if result[0] not in new_lines:
                    new_lines.append(result[0])
        print(new_lines)

def test_re():
    s = 'st_10 = "stablehlo.constant dense<0.210224107> : tensor<1x1x512x4096xf32>'
    result = pattern.findall(s)
    print(result)


# test_re()
find_and_count()
