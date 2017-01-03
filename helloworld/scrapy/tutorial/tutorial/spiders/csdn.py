# -*- coding: utf-8 -*-
import scrapy
from tutorial.items import TutorialItem

class CsdnSpider(scrapy.Spider):
    name = "csdn"
    allowed_domains = ["csdn.net"]
    start_urls = (
        'http://blog.csdn.net/antkillerfarm?viewmode=contents',
        'http://blog.csdn.net/antkillerfarm/article/list/2?viewmode=contents',
    )

    def parse(self, response):
        sel = scrapy.Selector(response)
        sites = sel.xpath('//div[@class="list_item list_view"]')
        items = []

        for site in sites:
            item = TutorialItem()
            item['title'] = site.xpath('descendant::span[@class="link_title"]/a/text()').extract()[0].encode("utf-8").strip()
            item['readCount'] = site.xpath('descendant::span[@class="link_view"]/text()').extract()[0].strip('()')
            items.append(item)
            print "title:="+item['title']
            print "readCount:="+item['readCount']

        return items
 
