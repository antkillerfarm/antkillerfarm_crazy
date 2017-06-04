#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import email
from email.header import decode_header
from bs4 import BeautifulSoup
import codecs

def eml2txt(file, fp):
    fp1 = open(file, "r")
    msg = email.message_from_file(fp1)
    fp1.close()
    for part in msg.walk():
        type = part.get_content_type()
        if type == "multipart/alternative":
            data = part.get("Subject")
            decoded_data = decode_header(data)
            (decoded_string, charset) = decoded_data[0]
            print decoded_string
            fp.write(decoded_string.decode('utf-8') + "\n")
        if type == "text/plain":
            data = part.get_payload(decode=True)
            soup = BeautifulSoup(data, from_encoding='utf-8')
            data = soup.get_text()
            lines = data.split("\n")
            for line in lines:
                nPos = line.find("via cnBeta.COM")
                if nPos >= 0:
                    break
                fp.write(line.encode('utf-8', 'ignore').decode('utf-8') + "\n")

dir_path = "/home/data/tj/cnbeta/11/"
des_path = "/home/data/tj/cnbeta/"

for dir in os.listdir(dir_path):
    fp2 = codecs.open(des_path + dir + ".txt", "w", "utf-8")
    for file in os.listdir(dir_path + dir):
        eml2txt(dir_path + dir + "/"+ file, fp2)
    fp2.close()


