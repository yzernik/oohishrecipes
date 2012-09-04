import scraper_caller


sc = scraper_caller.Scraper_Caller()
spider_names = ["allrecipes.com", "epicurious.com"]

for name in spider_names:
    #sc.scrape(name)
    sc.insert_in_db(name)
