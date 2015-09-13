#!/usr/bin/python3
import os
import sqlite3
import get_db_path

connection = sqlite3.connect(get_db_path.db_path())

cursor = connection.cursor()
cursor.execute("""CREATE TABLE wishes(
                    id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL,
                    name TEXT,
                    title TEXT,
                    wish TEXT NOT NULL,
                    contact_info NOT NULL,
                    time DATETIME NOT NULL,
                    thumb_up INTEGER NOT NULL)""")
connection.commit()
connection.close()
    
