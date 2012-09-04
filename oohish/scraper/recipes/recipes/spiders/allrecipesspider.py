# coding=utf-8

from scrapy.contrib.spiders import CrawlSpider, Rule
from scrapy.contrib.linkextractors.sgml import SgmlLinkExtractor
from scrapy.selector import HtmlXPathSelector
from scrapy.http import Request
from recipes.items import RecipeItem

class AllRecipesSpider(CrawlSpider):

    name = 'allrecipes.com'
    allowed_domains = ['allrecipes.com']
    start_urls = ['http://allrecipes.com/recipes/ViewAll.aspx']
    rules = [Rule(SgmlLinkExtractor(allow=['/recipe/[^/]+/detail.aspx']), callback='parse_recipe'),
             Rule(SgmlLinkExtractor(allow=['/recipes/ViewAll.aspx'], restrict_xpaths=('//a[text()="NEXT »"]',)))]


    def parse_recipe(self, response):
        x = HtmlXPathSelector(response)
        recipe = RecipeItem()
        recipe['url'] = response.url
        recipe['name'] = x.select("//span[@class='itemreviewed']/text()").extract()
        recipe['ingredients'] = x.select("//div[@class='ingredients']/ul/li/text()").extract()
        recipe['directions'] = x.select("//div[@class='directions']/ol/li/span/text()").extract()
        yield recipe
