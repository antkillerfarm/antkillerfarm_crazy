# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/en/latest/topics/item-pipeline.html
import json
import time

class TutorialPipeline(object):
    def __init__(self):
        filename=time.strftime("%Y_%m_%d")+".json"
        self.file = open(filename, "w+")

    def process_item(self, item, spider):
        record = json.dumps(dict(item), ensure_ascii=False)+"\n"
        #print "process_item"+record
        self.file.write(record)
        return item

    def open_spider(self, spider):
        pass
 
    def close_spider(self, spider):
        self.file.close()
