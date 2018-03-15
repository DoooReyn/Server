from pyquery import PyQuery as pq

html = '''
<div>
    <ul>
         <li class="item-0">first item</li>
         <li class="item-1"><a href="link2.html">second item</a></li>
         <li class="item-0 active"><a href="link3.html"><span class="bold">third item</span></a></li>
         <li class="item-1 active"><a href="link4.html">fourth item</a></li>
         <li class="item-0"><a href="link5.html">fifth item</a></li>
     </ul>
</div>
'''
# 这里我们可以知道上述代码中的doc其实就是一个pyquery对象，
# 我们可以通过doc可以进行元素的选择，其实这里就是一个css选择器，
# 所以CSS选择器的规则都可以用，直接doc(标签名)就可以获取所有的该标签的内容，
# 如果想要获取class 则doc('.class_name'),如果是id则doc('#id_name')....
doc = pq(html)
print(doc)
print(type(doc))
print(doc('li'))

print("==================")
html = '''
<div id="container">
    <ul class="list">
         <li class="item-0">first item</li>
         <li class="item-1"><a href="link2.html">second item</a></li>
         <li class="item-0 active"><a href="link3.html"><span class="bold">third item</span></a></li>
         <li class="item-1 active"><a href="link4.html">fourth item</a></li>
         <li class="item-0"><a href="link5.html">fifth item</a></li>
     </ul>
 </div>
'''
doc = pq(html)
print(doc('#container .list li'))


print("==================")
items = doc('.list')
print(type(items))
print(items)
lis = items.find('li')
print(type(lis))
print(lis)
# 从结果里我们也可以看出通过pyquery找到结果其实还是一个pyquery对象，可以继续查找，
# 上述中的代码中的items.find('li') 则表示查找ul里的所有的li标签
# 当然这里通过children可以实现同样的效果,并且通过.children方法得到的结果也是一个pyquery对象
li = items.children()
print(type(li))
print(li)

print("==================")
items = doc('.list')
container = items.parent()
print(type(container))
print(container)


#通过.parent就可以找到父元素的内容，例子如下：
print("==================")

html = '''
<div class="wrap">
    <div id="container">
        <ul class="list">
             <li class="item-0">first item</li>
             <li class="item-1"><a href="link2.html">second item</a></li>
             <li class="item-0 active"><a href="link3.html"><span class="bold">third item</span></a></li>
             <li class="item-1 active"><a href="link4.html">fourth item</a></li>
             <li class="item-0"><a href="link5.html">fifth item</a></li>
         </ul>
     </div>
 </div>
'''
doc = pq(html)
items = doc('.list')
parents = items.parents()
print(type(parents))
print(parents)


#同样我们通过.parents查找的时候也可以添加css选择器来进行内容的筛选
print("===================================")
html = '''
<div class="wrap">
    <div id="container">
        <ul class="list">
             <li class="item-0">first item</li>
             <li class="item-1"><a href="link2.html">second item</a></li>
             <li class="item-0 active"><a href="link3.html"><span class="bold">third item</span></a></li>
             <li class="item-1 active"><a href="link4.html">fourth item</a></li>
             <li class="item-0"><a href="link5.html">fifth item</a></li>
         </ul>
     </div>
 </div>
'''
from pyquery import PyQuery as pq
doc = pq(html)
li = doc('.list .item-0.active')
print(li)
print(li.siblings())

lis = doc('li').items()
print(type(lis))
for li in lis:
    print(type(li))
    print(li)

#pyquery对象.attr(属性名)
#pyquery对象.attr.属性名
a = doc('.item-0.active a')
print(a)
print(a.attr('href'))
print(a.attr.href)
print(a.text())
print(li.html())


