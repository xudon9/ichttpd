#!/usr/bin/python3
import sqlite3

db_name = 'wishdata.sqlite'

def get_wishes_from_store():
    connection = sqlite3.connect(db_name)
    cursor = connection.cursor()
    results = cursor.execute("""SELECT * FROM wishes""")
    response = results.fetchall()
    connection.close()
    return(response)
