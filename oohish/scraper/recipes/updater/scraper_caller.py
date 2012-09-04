import subprocess
import json
import psycopg2

# Connect to an existing database
conn = psycopg2.connect(dbname="oohish_db", user="oohish_admin")
# Open a cursor to perform database operations
cur = conn.cursor()


class Scraper_Caller:
    def scrape(self, spider_name):
        fn = "scraped_data_"+spider_name+".json"
        subprocess.call(["scrapy", "crawl", spider_name, "-o", fn, "-t", "json"])

        
    def insert_in_db(self, spider_name):
        fn = "scraped_data_"+spider_name+".json"
        with open(fn, 'r') as f:
            for line in f:
                line = line[line.index("{"):]
                line = line[:line.rindex("}")+1]
                recipe_dict = json.loads(line)
                recipe_name = recipe_dict['name'][0]
                recipe_url = recipe_dict['url']
                recipe_ingredients = recipe_dict['ingredients']
                recipe_directions = recipe_dict['directions']
                try:
                    cur.execute("INSERT INTO recipes (name, url) VALUES (%s, %s)", (recipe_name, recipe_url,))
                    conn.commit()
                    cur.execute("SELECT id FROM recipes WHERE url = %s;", (recipe_url,))
                    recipe = cur.fetchone()
                    recipe_id = recipe[0]

                    for ingredient in recipe_ingredients:
                        ingredient = ingredient.strip()
                        try:
                            cur.execute("INSERT INTO ingredients (content, recipe_id) VALUES (%s, %s)", (ingredient, recipe_id,))
                        except psycopg2.IntegrityError:
                            conn.rollback()

                    for direction in recipe_directions:
                        direction = direction.strip()
                        try:
                            cur.execute("INSERT INTO directions (content, recipe_id) VALUES (%s, %s)", (direction, recipe_id,))
                        except psycopg2.IntegrityError:
                            conn.rollback()
                except psycopg2.IntegrityError:
                    conn.rollback()
