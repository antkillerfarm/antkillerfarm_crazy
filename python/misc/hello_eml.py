#!/usr/bin/python
# -*- coding: utf-8 -*-

import email

dir_path = "/home/tj/big_data/eml/"
fp2 = open("/home/tj/big_data/eml/1.txt", "w")

fp1 = open("/home/tj/big_data/eml/1.eml", "r")
msg = email.message_from_file(fp1)
fp1.close()



for part in msg.walk():
    type = part.get_content_type()
    if type == "multipart/alternative":
        data = part.get("Subject")
        decoded_data = email.header.decode_header(data)
        (decoded_string, charset) = decoded_data[0]
        fp2.write(decoded_string + "\r\n")
    if type == "text/plain":
        data = part.get_payload(decode=True)
        lines = data.split("\n")
        for line in lines:
            nPos = line.find("via cnBeta.COM")
            if nPos >= 0:
                break
            fp2.write(line + "\r\n")

fp2.close()

