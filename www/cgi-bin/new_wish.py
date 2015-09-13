#!/usr/bin/python3
import yate
import wishmodel
import cgi
from change_wish_data import add_wish
import get_db_path

form_data = cgi.FieldStorage()

add_wish(form_data.getvalue('nickname'), form_data.getvalue('title'),
        form_data.getvalue('wish'), form_data.getvalue('contact_info'));

print('Content-type: text/html\n')

with open(get_db_path.root_dir() + 'templates/done_post.html') as done_postf:
  print(done_postf.read())