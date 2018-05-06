# -*- coding: utf-8 -*-
import scrapy
import string
import re
from tutorial.items import TutorialItem

class CsdnSpider(scrapy.Spider):
    name = "csdn"
    allowed_domains = ["csdn.net"]
    start_urls = (
        'https://blog.csdn.net/antkillerfarm/article/list/1','https://blog.csdn.net/antkillerfarm/article/list/2',
        'https://blog.csdn.net/antkillerfarm/article/list/3','https://blog.csdn.net/antkillerfarm/article/list/4',
        'https://blog.csdn.net/antkillerfarm/article/list/5','https://blog.csdn.net/antkillerfarm/article/list/6')

    def parse(self, response):
        sel = scrapy.Selector(response)
        sites = sel.xpath('//div[@class="article-item-box csdn-tracking-statistics"]')
        items = []
        num = 0

        for site in sites:
            item = TutorialItem()
            item['title'] = site.xpath('descendant::h4/a/text()').extract()[1].encode("utf-8").strip()
            item['readCount'] = site.xpath('descendant::span[@class="read-num"]/text()').extract()[0].strip().replace("阅读数：".encode("utf-8"),"")
            items.append(item)
            print "title:="+item['title']
            print "readCount:="+item['readCount']
            num += string.atoi(item['readCount'])

        print("Total Count: %d" % (num))
        return items
 
