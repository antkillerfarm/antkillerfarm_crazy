#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs
import re

fp1 = codecs.open("/home/tj/big_data/data/talk/u2u.txt", "r", "utf-8")
fp2 = codecs.open("/home/tj/big_data/data/talk/x2.txt", "w", "utf-8")

line =  fp1.readline()

while line:
    line1 =re.sub("http[s]?:\S*", "，TAGURL，".decode('utf-8'), line)
    line2 =re.sub("/:[^/]{2,4}", "，TAGEMOJI，".decode('utf-8'), line1)
    fp2.write(line2 + "\r\n")
    line =  fp1.readline()

fp2.close()
fp1.close()

#sed -e 's/.$//' 2.txt > u2.txt
#M-x set-buffer-file-coding-system
#sed -e 's/\r//g' u2.txt > u2u.txt

