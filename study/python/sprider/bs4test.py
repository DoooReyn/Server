from bs4 import BeautifulSoup

html = '''
<html><head><title>The Dormouse's story1</title></head>
<body>
<p class="title"  name="test"><b>The Dormouse's story2</b></p>

<p class="story">Once upon a time there were three little sisters; and their names were
<a href="http://example.com/elsie" class="sister" id="link1">Elsie</a>,
<a href="http://example.com/lacie" class="sister" id="link2">Lacie</a> and
<a href="http://example.com/tillie" class="sister" id="link3">Tillie</a>;
and they lived at the bottom of a well.</p>
<p class="story">...</p>
'''

soup = BeautifulSoup(html,'lxml')
# print(soup.prettify())
# print(soup.title)
# print(soup.title.name)
# print(soup.title.string)
# print(soup.title.parent.name)
# 通过这种soup.标签名 我们就可以获得这个标签的内容
# 这里有个问题需要注意，通过这种方式获取标签，
# 如果文档中有多个这样的标签，返回的结果是第一个标签的内容，
# 如上面我们通过soup.p获取p标签，而文档中有多个p标签，
# 但是只返回了第一个p标签内容
# print(soup.p)
# print(soup.p["class"])
# print(soup.a)
# print(soup.find_all('a'))
link = soup.find(class_='sister')
print(link)
print(link.name)
print(link.string)
print(link.get_text())
#当我们通过soup.title.name的时候就可以获得该title标签的名称，即title
print(soup.p.attrs['name'])
print(soup.p.get('name'))
print(soup.p['name'])
print(soup.p.string)
print(soup.head.title.string)


html = """
<html>
    <head>
        <title>The Dormouse's story</title>
    </head>
    <body>
        <p class="story">
            Once upon a time there were three little sisters; and their names were
            <a href="http://example.com/elsie" class="sister" id="link1">
                <span>Elsie</span>
            </a>
            <a href="http://example.com/lacie" class="sister" id="link2">Lacie</a>
            and
            <a href="http://example.com/tillie" class="sister" id="link3">Tillie</a>
            and they lived at the bottom of a well.
        </p>
        <p class="story">...</p>
"""

soup = BeautifulSoup(html,'lxml')
print(soup.p.contents)

print(soup.p.children)
for i,child in enumerate(soup.p.children):
    print(i,child)

print("==============================")
html='''
<div class="panel">
    <div class="panel-heading">
        <h4>Hello</h4>
    </div>
    <div class="panel-body">
        <ul class="list" id="list-1">
            <li class="element">Foo</li>
            <li class="element">Bar</li>
            <li class="element">Jay</li>
        </ul>
        <ul class="list list-small" id="list-2">
            <li class="element">Foo</li>
            <li class="element">Bar</li>
        </ul>
    </div>
</div>
'''

soup = BeautifulSoup(html, 'lxml')
print(soup.find_all('ul'))
print(type(soup.find_all('ul')[0]))

for ul in soup.find_all('ul'):
    print(ul.find_all('li'))

print("==============================")
html='''
<div class="panel">
    <div class="panel-heading">
        <h4>Hello</h4>
    </div>
    <div class="panel-body">
        <ul class="list" id="list-1" name="elements">
            <li class="element">Foo</li>
            <li class="element">Bar</li>
            <li class="element">Jay</li>
        </ul>
        <ul class="list list-small" id="list-2">
            <li class="element">Foo</li>
            <li class="element">Bar</li>
        </ul>
    </div>
</div>
'''
soup = BeautifulSoup(html, 'lxml')
print(soup.find_all(attrs={'id': 'list-1'}))
print(soup.find_all(attrs={'name': 'elements'}))
print(soup.find_all(text='Foo'))
print("==============================")
#通过select()直接传入CSS选择器就可以完成选择
#熟悉前端的人对CSS可能更加了解，其实用法也是一样的
#.表示class #表示id
#标签1，标签2 找到所有的标签1和标签2
#标签1 标签2 找到标签1内部的所有的标签2
#[attr] 可以通过这种方法找到具有某个属性的所有标签
#[atrr=value] 例子[target=_blank]表示查找所有target=_blank的标签

print(soup.select('.panel .panel-heading'))
print(soup.select('ul li'))
print(soup.select('#list-2 .element'))
print(type(soup.select('ul')[0]))

for li in soup.select('li'):
    print(li.get_text())

for ul in soup.select('ul'):
    print(ul['id'])
    print(ul.attrs['id'])
    print(ul.get("id"))
#推荐使用lxml解析库，必要时使用html.parser
#标签选择筛选功能弱但是速度快
#建议使用find()、find_all() 查询匹配单个结果或者多个结果
#如果对CSS选择器熟悉建议使用select()
#记住常用的获取属性和文本值的方法