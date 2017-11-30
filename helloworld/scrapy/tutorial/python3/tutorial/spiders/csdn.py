# -*- coding: utf-8 -*-
import scrapy
from tutorial.items import TutorialItem

class CsdnSpider(scrapy.Spider):
    name = "csdn"
    allowed_domains = ["csdn.net"]
    start_urls = (
        'http://blog.csdn.net/antkillerfarm/svc/getarticles?pageindex=1&pagesize=100',)

    def parse(self, response):
        sel = scrapy.Selector(response)
        sites = sel.xpath('//li[@class="blog-unit"]')
        items = []
        num = 0

        for site in sites:
            item = TutorialItem()
            item['title'] = site.xpath('descendant::h3[@class="blog-title odd-overhidden bottom-dis-8"]/a/text()').extract()[0].strip()
            item['readCount'] = site.xpath('descendant::div[@class=" floatL left-dis-24"]/span/text()').extract()[0].strip()
            items.append(item)
            print("title:=%s" % (item['title']))
            print("readCount:=%s" % (item['readCount']))
            num += int(item['readCount'])

        print("Total Count: %d" % (num))
        return items
 
