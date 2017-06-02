#!/usr/bin/python
# -*- coding: utf-8 -*-

import email

fp = open("/home/data/tj/cnbeta/x/1.eml", "r")
msg = email.message_from_file(fp)
fp.close()

for part in msg.walk():
    type = part.get_content_type()
    if type == "multipart/alternative":
        data = part.get("Subject")
        decoded_data = email.header.decode_header(data)
        (decoded_string, charset) = decoded_data[0]
        print decoded_string
    if type == "text/plain":
        data = part.get_payload(decode=True)
        lines = data.split("\n")
        for line in lines:
            nPos = line.find("via cnBeta.COM")
            if nPos >= 0:
                break
            print line

