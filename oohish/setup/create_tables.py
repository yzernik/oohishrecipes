import psycopg2

# Connect to an existing database
conn = psycopg2.connect(dbname="oohish_db", user="oohish_admin")

# Open a cursor to perform database operations
cur = conn.cursor()

# Execute a command: this creates a new table
# Create users table
cur.execute("CREATE TABLE users (id SERIAL PRIMARY KEY, name varchar(50) UNIQUE, password varchar(300), salt varchar(300));")
# Create items table
cur.execute("CREATE TABLE items (id SERIAL PRIMARY KEY, name varchar(50) UNIQUE);")
# Create user-items mappting table
cur.execute("CREATE TABLE user_item_mapping (user_id integer references users(id), item_id integer references items(id), count integer, PRIMARY KEY (user_id, item_id));")
# Create recipes table
cur.execute("CREATE TABLE recipes (id SERIAL PRIMARY KEY, name varchar(200), url varchar(200) UNIQUE);")
# Create ingredients table
cur.execute("CREATE TABLE ingredients (id SERIAL PRIMARY KEY, content varchar(500), recipe_id integer references recipes(id));")
# Create directions table
cur.execute("CREATE TABLE directions (id SERIAL PRIMARY KEY, content varchar(3000), recipe_id integer references recipes(id));")
# Create recipe-items mappting table
cur.execute("CREATE TABLE recipe_item_mapping (recipe_id integer references recipes(id), item_id integer references items(id), count integer, PRIMARY KEY (recipe_id, item_id));")



# Make the changes to the database persistent
conn.commit()

# Close communication with the database
cur.close()
conn.close()
