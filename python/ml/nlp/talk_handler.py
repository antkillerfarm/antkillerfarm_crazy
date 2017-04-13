import codecs
import re

def find_n_sub_str(src, sub, n, start=0):
    index = src.find(sub, start)
    if index != -1 and n > 0:
        return find_n_sub_str(src, sub, n - 1, index + 1)
    return index

def str_filter(str):
    res = str.replace(",", "，".decode('utf-8'))
    res = res.strip()
    return res

def line_filter(str):
    match = re.match("系统提示:".decode('utf-8'),str)
    if (match != None):
        return 1
    match = re.match("系统提醒:".decode('utf-8'),str)
    if (match != None):
        return 1
    return 0

fp1 = codecs.open("/home/tj/big_data/data/talk/x2.txt", "r", "utf-8")
fp2 = codecs.open("/home/tj/big_data/data/talk/c2.csv", "w", "utf-8")

line =  fp1.readline()
pos = line.find(":")
s_name = line[0:pos]
line =  fp1.readline()

flag = 0
while line:
    matchObj = re.match("[0-9]{4}-[0-9]{2}-[0-9]{2}",line)
    matchObj1 = re.match("[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}",line)
    if flag == 0 and matchObj != None and matchObj1 == None:
        flag = 1
        #fp2.write("TAG_START\r\n")
    pos = line.find(":")
    if (pos != -1):
        
        pos1 = find_n_sub_str(line, " ", 1)
        pos2 = find_n_sub_str(line, " ", 2)
        name = line[pos1:pos2]
        pos3 = name.find(s_name)
        talk = str_filter(line[pos2+1:])
        s = ""
        if (len(talk) > 5):
            if (pos3 != -1):
                s = "S,"
            else:
                s = "C,"
            if (flag == 1):
                s = s + "TAGSTART，".decode('utf-8')
                flag = 0
            s = s + talk + "\r\n"
            flag1 = line_filter(talk)
            if (flag1 == 0):
                fp2.write(s)
    
    line =  fp1.readline()

fp2.close()
fp1.close()

#print(re.match('[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}', '2017-02-21')) 
#print(re.sub("/:[^/]{2,4}", "，TAG_EMOJI，".decode('utf-8'),"/:^_^/:^_^".decode('utf-8')))

