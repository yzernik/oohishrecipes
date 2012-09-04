import psycopg2

import recipesearch

# Connect to an existing database
conn = psycopg2.connect(dbname="oohish_db", user="oohish_admin")
# Open a cursor to perform database operations
cur = conn.cursor()


class Tree_Builder:
    def build(self):
        cur.execute("SELECT id, content FROM ingredients;")
        for ingredient in cur:
            print ingredient[0], ingredient[1]
            ing_id = ingredient[0]
            ing_words = (filter(lambda c: c.isalnum() or c == ' ', ingredient[1])).split()
            for word in ing_words:
                recipesearch.add_string(word.lower(), ing_id)
