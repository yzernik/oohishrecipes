import psycopg2

import recipesearch


class Inventory(object):
    def __init__(self, conn, cur):
        self.conn = conn
        self.cur = cur

    def add_item(self, item_name):
        try:
            self.cur.execute("INSERT INTO items (name) VALUES (%s)", (item_name,))
            self.conn.commit()
        except psycopg2.IntegrityError:
            self.conn.rollback()
        self.cur.execute("SELECT * FROM items WHERE name = %s;", (item_name,))
        item = self.cur.fetchone()
        return item

    def get_item(self, item_name):
        self.cur.execute("SELECT * FROM items WHERE name = %s;", (item_name,))
        item = self.cur.fetchone()
        return item

    def add_user_item(self, user_id, item_id):
        try:
            self.cur.execute("INSERT INTO user_item_mapping (user_id, item_id) VALUES (%s, %s)", (user_id, item_id,))
            self.conn.commit()
        except psycopg2.IntegrityError:
            self.conn.rollback()
        return True

    def remove_user_item(self, user_id, item_id):
        try:
            self.cur.execute("DELETE FROM user_item_mapping WHERE user_item_mapping.user_id = %s AND user_item_mapping.item_id = %s;", (user_id, item_id,))
            self.conn.commit()
        except psycopg2.IntegrityError:
            self.conn.rollback()
        return 0

    def get_user_items(self, user_id):
        self.cur.execute("SELECT items.id, items.name FROM user_item_mapping, items WHERE user_item_mapping.item_id = items.id AND user_item_mapping.user_id = %s;", (user_id,))
        user_items = self.cur.fetchall()
        return user_items

    def get_user_thumbnail_items(self, user_id):
        thumbnail_items = self.get_user_items(user_id)
        return thumbnail_items[:20]

    def search_ingredient(self, ingredient_content):
        ing_words = (filter(lambda c: c.isalnum() or c == ' ', ingredient_content)).split()
        ing_results = {}
        for word in ing_words:
            word_result_ids = recipesearch.query_string(word.lower())
            for ing_id in word_result_ids:
                if not ing_id in ing_results:
                    ing_results[ing_id] = 1
                else:
                    ing_results[ing_id] += 1

        sorted_ing_ids = (sorted([k for k in ing_results], key=lambda x: ing_results[x]))
        sorted_ing_ids.reverse()
        threshold = len(ing_words)
        result_ingredients = []
        while(threshold > 0):
            threshold_ids = [k for k in ing_results if ing_results[k] == threshold]
            sql_str = "(" + ", ".join([str(x) for x in threshold_ids]) + ")" if threshold_ids else "(0)"
            query = "SELECT ingredients.id, ingredients.content FROM ingredients WHERE id IN %s;" % sql_str
            self.cur.execute(query)
            result_ingredients.extend(self.cur.fetchall())
            threshold -= 1
        return result_ingredients

    def get_recommended_recipes(self, user_id):
        item_projections = {}
        self.cur.execute("SELECT items.id, items.name FROM user_item_mapping, items WHERE user_item_mapping.item_id = items.id AND user_item_mapping.user_id = %s;", (user_id,))
        for record in self.cur:
            ing_words = (filter(lambda c: c.isalnum() or c == ' ', record.name)).split()
            ing_results = {}
            for word in ing_words:
                word_result_ids = recipesearch.query_string(word.lower())
                for ing_id in word_result_ids:
                    if not ing_id in ing_results:
                        ing_results[ing_id] = 1
                    else:
                        ing_results[ing_id] += 1
            word_threshold = len(ing_words)
            sufficient_ing_ids = [k for k in ing_results if ing_results[k] == word_threshold]
            item_projections[record.id] = set(sufficient_ing_ids)

        all_projections = set()
        for k in item_projections:
            all_projections = all_projections.union(item_projections[k])

        recipe_sets = {}
        self.cur.execute("SELECT ingredients.id, ingredients.recipe_id FROM ingredients;")
        for record in self.cur:
            if not record.recipe_id in recipe_sets:
                recipe_sets[record.recipe_id] = set()
            recipe_sets[record.recipe_id].add(record.id)

        recipe_measures = {}
        for k in recipe_sets:
            recipe_measures[k] = [len(recipe_sets[k]), len(recipe_sets[k].intersection(all_projections))]

        threshold = 0
        min_matches = 1
        result_recipes = []
        while(threshold > -10):
            threshold_ids = [k for k in recipe_measures if recipe_measures[k][1] >= min_matches and (recipe_measures[k][1] - recipe_measures[k][0]) == threshold]
            sql_str = "(" + ", ".join([str(x) for x in threshold_ids]) + ")" if threshold_ids else "(0)"
            query = "SELECT recipes.id, recipes.name FROM recipes WHERE id IN %s;" % sql_str
            self.cur.execute(query)
            result_recipes.extend(self.cur.fetchall())
            threshold -= 1
        return result_recipes


    def get_user_thumbnail_recipes(self, user_id):
        thumbnail_recipes = self.get_recommended_recipes(user_id)
        return thumbnail_recipes[:20]


    def get_recipe(self, recipe_id):
        self.cur.execute("SELECT recipes.id, recipes.name FROM recipes WHERE recipes.id = %s;", (recipe_id,))
        recipe = self.cur.fetchone()
        return recipe


    def get_recipe_ingredients(self, recipe_id):
        #query = "SELECT ingredients.id, ingredients.content FROM ingredients WHERE id IN %s;" % sql_str
        self.cur.execute("SELECT ingredients.id, ingredients.content FROM ingredients WHERE ingredients.recipe_id = %s;", (recipe_id,))
        recipe_ingredients = self.cur.fetchall()
        return recipe_ingredients
