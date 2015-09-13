#!/usr/bin/python3
import wishmodel
import cgi
import yate
from change_wish_data import add_thumb_up

form_data = cgi.FieldStorage()
info_id = form_data.keys()[0] #取得name,name本身代表了数据库里ID
add_thumb_up(info_id)

#写返回页面
print('Content-type: text/html\n')
with open('templates/done_thumb_up.html') as done_thumb_upf:
  print(done_thumb_upf.read())
