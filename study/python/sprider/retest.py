import re

# content= "hello 123 4567 World_This is a regex Demo"
# result = re.match('^hello\s\d\d\d\s\d{4}\s\w{10}.*Demo$',content)
# print(result)
# print(type(result.group()))
# print(result.group())
# print(result.span())
#
# content2= "hello 1234567 World_This is a regex Demo"
# result2= re.match('^he.*?(\d+).*Demo',content2)
# print(result2)
# print(result2.group(1))
#
#
#
# content3 = """hello 123456 world_this
# my name is zhaofan
# """
#
# result3 =re.match('^he.*?(\d+).*?zhaofan$',content3,re.S)
# print(result3)
# print(result3.group())
# print(result3.group(1))
#
# print("===================================")
# html = '''<div id="songs-list">
#     <h2 class="title">经典老歌</h2>
#     <p class="introduction">
#         经典老歌列表
#     </p>
#     <ul id="list" class="list-group">
#         <li data-view="2">一路上有你</li>
#         <li data-view="7"><a href="/2.mp3" singer="任贤齐">沧海一声笑</a></li>
#         <li data-view="4" class="active"><a href="/3.mp3" singer="齐秦">往事随风</a></li>
#         <li data-view="6"><a href="/4.mp3" singer="beyond">光辉岁月</a></li>
#         <li data-view="5"><a href="/5.mp3" singer="陈慧琳">记事本</a></li>
#         <li data-view="5"> <a href="/6.mp3" singer="邓丽君">但愿人长久</a></li>
#     </ul>
# </div>'''
#
#
# result = re.search('<li.*?active.*?singer="(.*?)">(.*?)</a>',html,re.S)
# print(result)
# print(result.groups())
# print(result.group(1))
# print(result.group(2))
#
#
#
# html = '''<div id="songs-list">
#     <h2 class="title">经典老歌</h2>
#     <p class="introduction">
#         经典老歌列表
#     </p>
#     <ul id="list" class="list-group">
#         <li data-view="2">一路上有你</li>
#         <li data-view="7"><a href="/2.mp3" singer="任贤齐">沧海一声笑</a></li>
#         <li data-view="4" class="active"><a href="/3.mp3" singer="齐秦">往事随风</a></li>
#         <li data-view="6"><a href="/4.mp3" singer="beyond">光辉岁月</a></li>
#         <li data-view="5"><a href="/5.mp3" singer="陈慧琳">记事本</a></li>
#         <li data-view="5"> <a href="/6.mp3" singer="邓丽君">但愿人长久</a></li>
#     </ul>
# </div>'''
#
# # results = re.findall('<li.*?href="(.*?)".*?singer="(.*?)">(.*?)</a>', html, re.S)
# # print(results)
# # print(type(results))
# # for result in results:
# #     #print(result)
# #     print(result[0], result[1], result[2])
#
# print('====================')
# results = re.findall('<li.*?>\s*?(<a.*?>)?(\w+)(</a>)?\s*?</li>',html,re.S)
# print(results)
# for result in results:
#     print(result[1])
#
# print('====================')
#
# content = "Extra things hello 123455 World_this is a regex Demo extra things"
#
# content = re.sub('\d+','',content)
# print(content)


#这里需要注意的一个问题是\1是获取第一个匹配的结果，为了防止转义字符的问题，我们需要在前面加上r
# content = "Extra things hello 123455 World_this is a regex Demo extra things"
#
# content = re.sub('(\d+)',r'\1 7890',content)
# print(content)
#
#
# content= """hello 12345 world_this
# 123 fan
# """
#
# pattern =re.compile("hello.*fan",re.S)
#
# result = re.match(pattern,content)
# print(result)
# print(result.group())
# print('====================')

import requests
content = requests.get('http://book.douban.com/').text
content = bytes(content,encoding="utf-8")

#print(type(content))

content = content.decode('GBK','ignore')

print(type(content))

exit(0)

#pattern = re.compile('<li.*?cover.*?href="(.*?)".*?title="(.*?)".*?more-meta.*?author">(.*?)</span>.*?year">(.*?)</span>.*?</li>', re.S)
#print('====================')

fp = open("douban.html","w")
fp.write(content)

exit(0)

results = re.findall(pattern, content)
print(results)


for result in results:
    url,name,author,date = result
    author = re.sub('\s','',author)
    date = re.sub('\s','',date)
    print(url,name,author,date)