#!/usr/bin/python3
import sqlite3
import sys,os
import get_db_path

def cur_file_dir():
    path = sys.path[0]
    if os.path.isdir(path):
        return path
    elif os.path.isfile(path):
        return os.path.dirname(path)

db_name = get_db_path.db_path()

def get_wishes_from_store():
    connection = sqlite3.connect(db_name)
    cursor = connection.cursor()
    results = cursor.execute("""SELECT * FROM wishes""")
    response = results.fetchall()
    connection.close()
    return(response)
