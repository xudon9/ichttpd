#偷懒暂时没用正则表达式…… 由于此字符串不是用户输入，所以不会出错
def to_human_time(str):
    year = str[0:4]
    month= str[5:7]
    day = str[8:10]

    hour = str[11:13]
    minite = str[14:16]
    second = str[17:19]
    return '许愿时间: '+year+'年'+month+'月'+day+'日 '+hour+'时'+minite+'分'+second+'秒'

