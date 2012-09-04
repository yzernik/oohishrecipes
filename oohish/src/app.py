from paste.urlparser import StaticURLParser
from paste.cascade import Cascade
from paste import httpserver
import webapp2
from webapp2_extras import sessions
from webapp2_extras import jinja2
import psycopg2
import psycopg2.extras
import random

import auth
import inventory

class BaseHandler(webapp2.RequestHandler):
  def dispatch(self):
    # Get a session store for this request.
    self.session_store = sessions.get_store(request=self.request)
    try:
      # Dispatch the request.
      webapp2.RequestHandler.dispatch(self)
    finally:
      # Save all sessions.
      self.session_store.save_sessions(self.response)

  @webapp2.cached_property
  def session(self):
    # Returns a session using the default cookie key.
    return self.session_store.get_session()
  
  @webapp2.cached_property
  def jinja2(self):
    # Returns a Jinja2 renderer cached in the app registry.
    return jinja2.get_jinja2(app=self.app)
  
  def render_response(self, _template, **context):
    # Renders a template and writes the result to the response.
    rv = self.jinja2.render_template(_template, **context)
    self.response.write(rv)


class DBHandler(BaseHandler):
  def __init__(self, request=None, response=None):
    BaseHandler.__init__(self, request, response)
    self.dbname = "oohish_db"
    self.user = "oohish_admin"
    self.conn = psycopg2.connect(dbname=self.dbname, user=self.user)
    self.cur = self.conn.cursor(cursor_factory=psycopg2.extras.NamedTupleCursor)

  def get_users(self):
    self.cur.execute("SELECT * FROM users;")
    return self.cur.fetchall()


class UserAwareHandler(DBHandler):
  def __init__(self, request=None, response=None):
    DBHandler.__init__(self, request, response)

  def set_user(self, user):
    self.session['user'] = user

  def get_user(self):
    return self.session.get('user')

  def unset_user(self):
    self.session['user'] = None

  def render_response(self, _template, **context):
    user = self.get_user()
    new_context = {'user': user}
    new_context.update(context)
    rv = self.jinja2.render_template(_template, **new_context)
    self.response.write(rv)


def login_required(handler):
  "Requires that a user be logged in to access the resource"
  def check_login(self, *args, **kwargs):     
    if not self.user:
      return self.redirect_to('login')
    else:
      return handler(self, *args, **kwargs)
  return check_login


class HomeHandler(UserAwareHandler):
  def get(self):
    user = self.get_user()
    if user:
      return self.redirect('/profile')
    context = {}
    self.render_response('home_page.html', **context)


class AboutHandler(UserAwareHandler):
  def get(self):
    context = {}
    self.render_response('about_page.html', **context)


class ContactHandler(UserAwareHandler):
  def get(self):
    context = {}
    self.render_response('contact_page.html', **context)


class SigninHandler(UserAwareHandler):
  def get(self):
    user = self.get_user()
    if user:
      return self.redirect('/')
    context = {}
    self.render_response('signin_page.html', **context)

  def post(self):
    user = self.get_user()
    if user:
      return self.redirect('/')
    username = self.request.POST["username"]
    password = self.request.POST["password"]
    authenticator = auth.UserAuth(self.conn, self.cur)
    user = authenticator.get_user(username, password)
    if user:
      self.set_user(user)
      return self.redirect('/profile')
    else:
      return self.redirect('/signin')


class SignupHandler(UserAwareHandler):
  def get(self):
    user = self.get_user()
    if user:
      return self.redirect('/')
    context = {}
    self.render_response('signup_page.html', **context)

  def post(self):
    user = self.get_user()
    if user:
      return self.redirect('/')
    username = self.request.POST["username"]
    password = self.request.POST["password"]
    repeat_password = self.request.POST["repeat_password"]
    if password != repeat_password:
      return self.response.write("Passwords don't match.")
    authenticator = auth.UserAuth(self.conn, self.cur)
    new_user = authenticator.create_user(username, password)
    if new_user:
      self.set_user(new_user)
      return self.redirect('/profile')
    else:
      return self.redirect('/signup')


class SignoutHandler(UserAwareHandler):
  def get(self):
    self.unset_user()
    return self.redirect('/')


class ListUsersHandler(UserAwareHandler):
  def get(self):
    u = DBHandler.get_users(self)
    context = {'users': u}
    self.render_response('users_page.html', **context)


class ProfileHandler(UserAwareHandler):
  def get(self):
    user = self.get_user()
    if not user:
      return self.redirect('/')
    inv = inventory.Inventory(self.conn, self.cur)
    thumbnail_items = inv.get_user_thumbnail_items(user['id'])
    thumbnail_recipes = inv.get_user_thumbnail_recipes(user['id'])
    context = {"thumbnail_items": thumbnail_items, "thumbnail_recipes": thumbnail_recipes}
    self.render_response('profile_page.html', **context)


class MyItemsHandler(UserAwareHandler):
  def get(self):
    user = self.get_user()
    if not user:
      return self.redirect('/')
    suggest_item_name = self.request.GET["suggest"] if "suggest" in self.request.GET else ""
    inv = inventory.Inventory(self.conn, self.cur)
    my_items = inv.get_user_items(user['id'])
    context = {"my_items": my_items, "suggest_item_name": suggest_item_name}
    self.render_response('my_items_page.html', **context)


class AddItemHandler(UserAwareHandler):
  def post(self):
    user = self.get_user()
    if not user:
      return self.redirect('/')
    item_name = self.request.POST["item_name"]
    inv = inventory.Inventory(self.conn, self.cur)
    item = inv.add_item(item_name)
    item_id = item.id
    inv.add_user_item(user['id'], item_id)
    return self.redirect('/myitems')


class RemoveItemHandler(UserAwareHandler):
  def get(self):
    user = self.get_user()
    if not user:
      return self.redirect('/')
    suggest_item_name = self.request.GET["name"] if "name" in self.request.GET else ""
    context = {"suggest_item_name": suggest_item_name}
    self.render_response('remove_item_page.html', **context)

  def post(self):
    user = self.get_user()
    if not user:
      return self.redirect('/')
    item_name = self.request.POST["item_name"]
    inv = inventory.Inventory(self.conn, self.cur)
    item = inv.get_item(item_name)
    item_id = item.id
    inv.remove_user_item(user['id'], item_id)
    return self.redirect('/myitems')


class SearchIngredientHandler(UserAwareHandler):
  def post(self):
    user = self.get_user()
    if not user:
      return self.redirect('/')
    ingredient_name = self.request.POST["ingredient_name"]
    inv = inventory.Inventory(self.conn, self.cur)
    search_ingredients = inv.search_ingredient(ingredient_name)
    context = {"search_ingredients": search_ingredients}
    self.render_response('search_ingredient_page.html', **context)


class RecommendedRecipesHandler(UserAwareHandler):
  def get(self):
    user = self.get_user()
    if not user:
      return self.redirect('/')
    inv = inventory.Inventory(self.conn, self.cur)
    recommended_recipes = inv.get_recommended_recipes(user['id'])
    context = {"recommended_recipes": recommended_recipes}
    self.render_response('recommended_recipes_page.html', **context)


class RecipeHandler(UserAwareHandler):
  def get(self):
    user = self.get_user()
    if not user:
      return self.redirect('/')
    if not "id" in self.request.GET:
      return self.redirect('/')

    recipe_id = self.request.GET["id"]
    inv = inventory.Inventory(self.conn, self.cur)
    recipe = inv.get_recipe(recipe_id)
    recipe_ingredients = inv.get_recipe_ingredients(recipe_id)
    
    context = {"recipe": recipe, "recipe_ingredients": recipe_ingredients}
    self.render_response('recipe_page.html', **context)


# Set the secret key
config = {}
secret_key = str(random.getrandbits(32))
config['webapp2_extras.sessions'] = {
    'secret_key': secret_key,
}

# Create the main app    
web_app = webapp2.WSGIApplication([
    ('/', HomeHandler),
    ('/home', HomeHandler),
    ('/about', AboutHandler),
    ('/contact', ContactHandler),
    ('/signin', SigninHandler),
    ('/signup', SignupHandler),
    ('/signout', SignoutHandler),
    ('/listusers', ListUsersHandler),
    ('/profile', ProfileHandler),
    ('/myitems', MyItemsHandler),
    ('/additem', AddItemHandler),
    ('/removeitem', RemoveItemHandler),
    ('/searchingredient', SearchIngredientHandler),
    ('/recommendedrecipes', RecommendedRecipesHandler),
    ('/recipe', RecipeHandler),
    ], config=config, debug=True)

# Create an app to serve static files
# Choose a directory separate from your source (e.g., "static/")
static_app = StaticURLParser("static/")

# Create a cascade that looks for static files first, then tries the webapp
app = Cascade([static_app, web_app])

def main():
    httpserver.serve(app, host='127.0.0.1', port='8080')

if __name__ == '__main__':
    main()
