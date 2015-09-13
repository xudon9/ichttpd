#!/usr/bin/python3
import yate
import wishmodel

wishes = wishmodel.get_wishes_from_store()
#时间倒序排序
wishes.sort(key = lambda x:x[5], reverse=1)
#生成页面
print(yate.start_response())
print(yate.include_header("心愿墙"))
boxes = yate.include_wishes_form(wishes)
print(yate.include_footer(boxes))
