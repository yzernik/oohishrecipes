# coding=utf-8

from scrapy.contrib.spiders import CrawlSpider, Rule
from scrapy.contrib.linkextractors.sgml import SgmlLinkExtractor
from scrapy.selector import HtmlXPathSelector
from scrapy.http import Request
from recipes.items import RecipeItem

class EpicuriousSpider(CrawlSpider):

    name = 'epicurious.com'
    allowed_domains = ['epicurious.com']
    start_urls = ['http://www.epicurious.com/tools/searchresults/all?search=']
    rules = [Rule(SgmlLinkExtractor(allow=['/recipes/[^/]+/views/[^/]+']), callback='parse_recipe'),
             Rule(SgmlLinkExtractor(allow=['http://www.epicurious.com/tools/searchresults/all'], restrict_xpaths=('//a[text()="next"]',)))]


    def parse_recipe(self, response):
        x = HtmlXPathSelector(response)
        recipe = RecipeItem()
        recipe['url'] = response.url
        recipe['name'] = x.select("//h1[@class='fn']/text()").extract()
        recipe['ingredients'] = x.select("//ul[@class='ingredientsList']/li/text()").extract()
        recipe['directions'] = x.select("//p[@class='instruction']/text()").extract()
        yield recipe
