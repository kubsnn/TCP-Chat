from math import e
import re
import sqlite3
import os

APP_NAME = "TCP-Chat"

class MessagesDB:
    def __init__(self, username):
        self.database = os.path.join(self.get_user_directory(), "chat_" + str(username) + ".db")
        self.conn = sqlite3.connect(self.database, timeout=10)
        self.c = self.conn.cursor()

        self.init_table()

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
        try:
            print("Add message:",contact, message, am_i_sender)
            if self.c is not None and self.conn is not None:
                self.c.execute("INSERT INTO messages (username, message, am_i_sender) VALUES (?, ?, ?)", (contact, message, am_i_sender))
                self.conn.commit()
                return True
        except Exception as e:
            print(e)
            return False

    def get_messages(self, recipient = None):
        try:
            if self.c is not None and self.conn is not None:
                if recipient is None:
                    self.c.execute("SELECT * FROM messages ORDER BY timestamp ASC")
                else:
                    self.c.execute("SELECT * FROM messages WHERE username = ? ORDER BY timestamp ASC", (recipient,))
                return self.c.fetchall()
        except Exception as e:
            print(e)
            return False

if __name__ == "__main__":
    messagesDB = MessagesDB("test")
    messagesDB.insert_message("test2", "test", True)
    messagesDB.insert_message("test2", "test", False)
    print(messagesDB.get_messages("test2"))