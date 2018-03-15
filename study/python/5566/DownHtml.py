#!/usr/bin/python
# -*- coding: utf-8 -*-
import requests
import os
import re
from bs4 import BeautifulSoup
import platform
import threadpool


# 网页内容下载保存到文件
def GetHtml(url, filename, proxy=None):

    if os.path.exists(filename):
        return 200

    my_header = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36',
        'Cache-Control': 'max-age=0',
        'Upgrade-Insecure-Requests': '1',
        'Connection': 'keep-alive',
        'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
        'Accept-Encoding': 'gzip, deflate, sdch',
        'Accept-Language': 'zh-CN,zh;q=0.8',
        'Cookie': '_ga=GA1.2.372156574.1517047678'
    }
    r = requests.get(url, headers=my_header, proxies=proxy)
    print("下载------%s" % url)
    # print(r.url)
    # 打印返回内容
    # print(r.content)
    if r.status_code != 200:
        return r.status_code

    fp = open(filename, 'wb+')
    content = r.content

    if platform.system() == "Linux":
        content = r.content.decode('gbk').encode('utf-8')

    fp.write(content)
    fp.close()

    return r.status_code


def GetPageNum(url, filename):
    code = GetHtml(url, filename)
    if code != 200:
        print("GetPageNum error")

    soup = BeautifulSoup(open(filename), 'lxml')
    # print(dir(soup))
    pageall = soup.find_all(class_='page')
    page = pageall[0]
    content = page.find_all("span")[0].get_text()
    # print(content)
    reobj = re.search(r':1/(\d+)\w', content)
    pagenum = reobj.group(1)
    return int(pagenum)


def GetItemUrl(url, filename):
    reobj = re.match(r'(http://)(.+?)/', url)
    host = ""
    if reobj:
        host = reobj.group(1) + reobj.group(2)

    urllist = []
    code = GetHtml(url, filename)
    if code != 200:
        print("GetItemUrl error")

    soup = BeautifulSoup(open(filename), 'lxml')
    item = soup.find(class_='list')
    trlist = item.find_all('tr')
    for tr in trlist:
        href = tr.find("a").get("href")
        urllist.append(host + href)
    return urllist


def GetImgUrl(url, filename):
    imgurllist = []
    code = GetHtml(url, filename)
    if code != 200:
        print("GetImgUrl error")

    soup = BeautifulSoup(open(filename), 'lxml')
    ks = soup.find("div", id="ks")
    title = ks.find("h1").get_text()
    # print(title)
    imglist = ks.find_all("img")
    for img in imglist:
        url = img.get("src")
        url.replace("https", "http")
        imgurllist.append(url)
    return title, imgurllist


def DownLoadImg(url, index, curdir):
    if url is None:
        print("url is none")
        return

    extname = url.split('.')[-1]
    image_name = str(index) + "." + extname

    if not os.path.exists(curdir + "/" + image_name):
        print(u'下载图片-----%s' % image_name)
        r = requests.get(url)
        with open(curdir + "/" + image_name, 'wb') as f:
            f.write(r.content)
    else:
        print(u'文件已经存在')
    return


def DownLoadPage(title, imglist, dirname):
    curdir = dirname + title
    if not os.path.exists(curdir):
        os.mkdir(curdir)

    index = 0
    for url in imglist:
        index = index + 1
        DownLoadImg(url, index, curdir)


def DownLoadIndex(index, filename, dirname):
    url = "http://www.xfyy24.com/html/picture1/"
    if index > 1:
        url = ("http://www.xfyy24.com/html/picture1/index%d.html") % index

    # print(url)
    # print(filename)

    indexhtmldir = dirname + "html/" + str(index) + "/"
    if not os.path.exists(indexhtmldir):
        os.makedirs(indexhtmldir)

    # print(indexhtmldir)

    itemlist = GetItemUrl(url, filename)

    itemindex = 0
    for url in itemlist:
        itemindex = itemindex + 1
        file = indexhtmldir + str(itemindex) + ".html"
        imglist = GetImgUrl(url, file)
        title = imglist[0]
        imgurlist = imglist[1]
        DownLoadPage(title, imgurlist, dirname)


def Main():
    filename = "picture1.html"
    url = "http://www.xfyy24.com/html/picture1/"
    dirname = "./ooxx/"
    pagedir = "./pagehtml/"
    if not os.path.exists(dirname):
        os.mkdir(dirname)
    if not os.path.exists(pagedir):
        os.mkdir(pagedir)

    pagenum = GetPageNum(url, filename)
    print("pagenum:%d" % pagenum)

    pagelist = []
    for i in range(0, 10):
        index = i + 1
        filename = (pagedir + "page_%d.html" % index)
        pagelist.append(([index, filename, dirname], None))

    #print(pagelist)

    pool = threadpool.ThreadPool(10)
    requests = threadpool.makeRequests(DownLoadIndex, pagelist)
    [pool.putRequest(req) for req in requests]
    pool.wait()


Main()
