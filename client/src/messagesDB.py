from math import e
import re
import sqlite3
import os
import hashlib
import env

APP_NAME = env.APP_NAME

class MessagesDB:
    def __init__(self, username):
        self.username_hash = self.hash_username(username, "messages_")

        self.database = os.path.join(self.get_user_directory(), self.username_hash + ".db")
        self.conn = sqlite3.connect(self.database, timeout=10)
        self.c = self.conn.cursor()

        self.init_table()

    def hash_username(self, username, salt) -> str:
        salted_username = salt + username
        sha256 = hashlib.sha256()
        sha256.update(salted_username.encode('utf-8'))
        hashed_username = sha256.hexdigest()
        return hashed_username



    def get_user_directory(self):

        # Get the user data directory
        if os.name == 'nt':  # Windows
            user_data_dir = os.getenv('APPDATA') or ""
        else:  # Linux and others
            user_data_dir = os.path.expanduser("~")

        if user_data_dir is None or user_data_dir == "":
            raise Exception("Could not get user data directory")

        app_data_dir = os.path.join(str(user_data_dir), str(APP_NAME))

        # Ensure the directory exists; if not, create it
        os.makedirs(app_data_dir, exist_ok=True)

        return app_data_dir

    def close(self):
        try:
            if self.conn is not None:
                self.conn.close()
                self.conn = None
                self.c = None
            return True
        except Exception as e:
            print(e)
            return False

    def init_table(self):
        try:
            if self.c is not None:

                self.c.execute('''CREATE TABLE IF NOT EXISTS messages
                    (id INTEGER PRIMARY KEY AUTOINCREMENT,
                    username TEXT,
                    message TEXT,
                    am_i_sender BOOLEAN,
                    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)''')

                return True
        except Exception as e:
            print(e)
            return False

    def insert_message(self, contact, message, am_i_sender):
        hashed_contact = self.hash_username(contact, self.username_hash)
        try:
            if self.c is not None and self.conn is not None:
                #get last id
                self.c.execute("SELECT id FROM messages ORDER BY id DESC LIMIT 1")
                last_id = self.c.fetchone()
                if last_id is None:
                    last_id = 0
                else:
                    last_id = last_id[0]

                #hashed_contact = self.hash_username(contact, self.username_hash + str(last_id))

                self.c.execute("INSERT INTO messages (username, message, am_i_sender) VALUES (?, ?, ?)", (hashed_contact, message, am_i_sender,))
                self.conn.commit()
                return True
        except Exception as e:
            print(e)
            return False

    def get_messages(self, recipient):
        try:
            if self.c is not None and self.conn is not None:
                hashed_recipient = self.hash_username(recipient, self.username_hash)
                self.c.execute("SELECT * FROM messages WHERE username = ? ORDER BY timestamp ASC", (str(hashed_recipient),))
                return self.c.fetchall()
        except Exception as e:
            print(e)
            return False

if __name__ == "__main__":
    messagesDB = MessagesDB("test")
    messagesDB.insert_message("test2", "test", True)
    messagesDB.insert_message("test2", "test", False)
    print(messagesDB.get_messages("test2"))