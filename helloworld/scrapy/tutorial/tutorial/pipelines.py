# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/en/latest/topics/item-pipeline.html
import json
import sys

class TutorialPipeline(object):
    def __init__(self):
        reload(sys)
        sys.setdefaultencoding("utf8")
        self.file = open("./item1.json", "w+")

    def process_item(self, item, spider):
        record = json.dumps(dict(item), ensure_ascii=False)+"\n"
        #print "process_item"+record
        self.file.write(record)
        return item

    def open_spider(self, spider):
        pass
 
    def close_spider(self, spider):
        self.file.close()
