import psycopg2
import hashlib, uuid


class UserAuth(object):
    def __init__(self, conn, cur):
        self.conn = conn
        self.cur = cur

    def create_user(self, username, password):
        salt = uuid.uuid4().hex
        hashed_password = hashlib.sha256(password + salt).hexdigest()
        try:
            self.cur.execute("INSERT INTO users (name, password, salt) VALUES (%s, %s, %s)", (username, hashed_password, salt,))
            self.conn.commit()
            self.cur.execute("SELECT * FROM users WHERE name = %s;", (username,))
            user = self.cur.fetchone()
            return user
        except psycopg2.IntegrityError:
            return None

    def get_user(self, username, password):
        self.cur.execute("SELECT * FROM users WHERE name = %s;", (username,))
        user = self.cur.fetchone()
        if not user:
            return None
        valid_hash = user.password
        salt = user.salt
        test_hash = hashlib.sha256(password + salt).hexdigest()
        if valid_hash != test_hash:
            return None
        return user
