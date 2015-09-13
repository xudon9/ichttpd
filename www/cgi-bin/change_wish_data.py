#!/usr/bin/python3
import sqlite3
import datetime,time
import get_db_path

def add_wish(name, title, wish, contact_info, ctime=time.localtime(), thumb_up = 0):
  format_time = time.strftime('%Y-%m-%d %H:%M:%S',ctime)
  connection = sqlite3.connect(get_db_path.db_path())
  cursor = connection.cursor()
  cursor.execute("""INSERT INTO wishes(title, name, wish, contact_info, time, thumb_up)
    VALUES (?,?,?,?,?,?)""", (title, name, wish, contact_info, format_time, thumb_up))
  connection.commit()
  connection.close()

def add_thumb_up(info_id):
    connection = sqlite3.connect(get_db_path.db_path())
    cursor = connection.cursor()
    cursor.execute("""UPDATE wishes SET thumb_up=thumb_up+1 WHERE id=?""", (info_id,))
    connection.commit()
    connection.close()
