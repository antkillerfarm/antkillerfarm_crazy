import codecs

def find_n_sub_str(src, sub, n, start=0):
    index = src.find(sub, start)
    if index != -1 and n > 0:
        return find_n_sub_str(src, sub, n - 1, index + 1)
    return index

fp1 = codecs.open("/home/tj/big_data/data/talk/u1u.txt", "r", "utf-8")
fp2 = codecs.open("/home/tj/big_data/data/talk/c1.txt", "w", "utf-8")

line =  fp1.readline()
pos = line.find(":")
s_name = line[0:pos]
line =  fp1.readline()

while line:
    pos = line.find(":")
    if (pos != -1):
        pos1 = find_n_sub_str(line, " ", 1)
        pos2 = find_n_sub_str(line, " ", 2)
        name = line[pos1:pos2]
        pos3 = name.find(s_name)
        talk = line[pos2+1:].replace(",", "，".decode('utf-8'))
        if (len(line) - pos2 > 2):
            if (pos3 != -1):
                fp2.write("S," + talk)
            else:
                fp2.write("C," + talk)
    
    line =  fp1.readline()

fp2.close()
fp1.close()

