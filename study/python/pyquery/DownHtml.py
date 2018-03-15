#!/usr/bin/python
# -*- coding: utf-8 -*-
import requests
import os
import re
from pyquery import PyQuery as pq
import hashlib
import base64
import platform


def parse(imgHash, constant):
    q = 4
    hashlib.md5()
    constant = md5(constant)
    o = md5(constant[0:16])
    n = md5(constant[16:32])
    l = imgHash[0:q]
    c = o + md5(o + l)
    imgHash = imgHash[q:]
    k = decode_base64(imgHash)
    h = list(range(256))

    b = list(range(256))

    for g in range(0, 256):
        b[g] = ord(c[g % len(c)])

    f = 0
    for g in range(0, 256):
        f = (f + h[g] + b[g]) % 256
        tmp = h[g]
        h[g] = h[f]
        h[f] = tmp

    result = ""
    p = 0
    f = 0
    for g in range(0, len(k)):
        p = (p + 1) % 256
        f = (f + h[p]) % 256
        tmp = h[p]
        h[p] = h[f]
        h[f] = tmp
        result += chr(k[g] ^ (h[(h[p] + h[f]) % 256]))
    result = result[26:]

    return result


def md5(src):
    m = hashlib.md5()
    m.update(src.encode("utf8"))
    return m.hexdigest()


def decode_base64(data):
    missing_padding = 4 - len(data) % 4
    if missing_padding:
        data += '=' * missing_padding
    return base64.b64decode(data)


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

    sysstr = platform.system()
    if(sysstr == "Windows"):
        tempfile = filename + "_bak"
        fp = open(tempfile, 'wb+')
        fp.write(r.content)
        fp.close()

        cmd = "iconv  -f UTF-8 -t gbk  -c %s > %s" % (tempfile, filename)
        os.system(cmd)
        cmd = "rm -f %s" % tempfile
        os.system(cmd)

        print("下载------%s------成功" % url)

    else:
        fp = open(filename, 'wb+')
        fp.write(r.content)
        fp.close()

    return r.status_code


# 检查代理是否有效
def CheckProxy(ipport):
    proxy = {'http': 'http://%s' % ipport}
    # print(proxy)
    try:
        r = requests.get("http://www.baidu.com", timeout=10, proxies=proxy)
        status_code = r.status_code
    except Exception as e:
        if 'time' in str(e):
            return False
        else:
            return False
    return status_code == 200
# 查找代理ip和port 内容保存到文件


def GetProxy(filename):
    if not os.path.exists(filename):
        GetHtml('http://www.xicidaili.com/nn', filename)

    proxy = []
    doc = pq(filename=filename)

    # print doc('title').text()
    # print doc('tr').size()
    # for tr in doc('tr').items():
    # print tr.size()
    # print tr.text()
    # print "==============="
    # for i in tr('.odd').items():
    # print i.size()
    # print i.eq(0).text()
    # ip = i.eq(0).text().split()[0]
    # port = i.eq(0).text().split()[1]
    # proxy.append(ip + ":" + port)
    # item = i.text().split()
    # ip = item[0]
    # port = item[1]
    # proxy.append(ip + ":" + port)

    for tr in doc('tr').items():
        item = tr.text().split()
        ip = item[0]
        port = item[1]
        matchobj = re.match("\d{1,3}\\.\d{1,3}\\.\d{1,3}\\.\d{1,3}", ip)
        # print matchobj
        if matchobj is not None:
            #proxy.append(ip + ":" + port)
            bused = CheckProxy(ip + ":" + port)
            if bused is True:
            proxy.append(ip + ":" + port)

    return proxy


def GetPageNum(filename):
    code = GetHtml('http://jandan.net/ooxx', filename)
    if code != 200:
        ip_list = GetProxy('daili.html')
        proxy = {'http': 'http://%s' % ip_list[0]}
        code = GetHtml('http://jandan.net/ooxx', filename, proxy=proxies)
    else:
        pass
    doc = pq(filename=filename)
    current_page_num = doc('.current-comment-page')
    page_num = current_page_num.eq(0).text().strip('[]')
    return int(page_num)


def GetImageUrl(filename, dirname, index):
    hastlist = []
    jsurl = ""
    htmlfile = dirname + "/" + filename
    doc = pq(filename=htmlfile)
    # content = doc.outer_html()
    # jsreobj=re.search(r'<script\Wsrc=\"\/\/(cdn.jandan.net/static/min.+?)\">',content);
    # jsFileUrl="http://"+j.group(1)
    # print(jsFileUrl)

    for jsscript in doc('script'):
        js = jsscript.get('src')
        if(type(js) is str):
            rematch = re.search(r'cdn.jandan.net/static/min', js)
            if rematch:
                jsurl = "http:" + js
                break
    if len(jsurl) == 0:
        print("jsurl find error")
        return

    jsfile = "%s/%d.js" % (dirname, index)
    status_code = GetHtml(jsurl, jsfile)
    if status_code != 200:
        print("jsurl link error")
        return

    jsdoc = pq(filename=jsfile)
    jscontent = jsdoc.outer_html()
    research = re.search(r'c=f_\w+\(e,\"(.+?)\"\)', jscontent)
    if research is None:
        print("jsurl file key get error")
        return

    hashkey = research.group(1)

    for img in doc('.img-hash').items():
        img_hash = img.text()
        hastlist.append(img.text())

    return hashkey, hastlist


def DownLoadImg(url, dirname):
    if url is None:
        print("url is none")
        return

    image_name = url.split('/')[-1]
    if len(image_name) == 0:
        print(u'%s 出现了问题' % url)
        return

    if not os.path.exists(dirname + "/" + image_name):
        print(u'下载图片-----%s' % image_name)
        r = requests.get(url)
        with open(dirname + "/" + image_name, 'wb') as f:
            f.write(r.content)
    else:
        print(u'文件已经存在')

    return


def ParseImgUrl(hashvalue, key):
    url = parse(hashvalue, key)
    #print("imgurl:%s" % url)

    url = 'http:' + url
    # c.replace(/(\/\/\w+\.sinaimg\.cn\/)(\w+)(\/.+\.(gif|jpg|jpeg))/, "$1large$3")
    replace = re.match(r'(.*\.sinaimg\.cn\/)(\w+)(\/.+\.(gif|jpg|jpeg))', url)
    if replace:
        url = replace.group(1) + 'large' + replace.group(3)

    return url


def StartDownLoad(htmlfile, dirname, index):
    result = GetImageUrl(htmlfile, dirname, index)
    for hashvale in result[1]:
        url = ParseImgUrl(hashvale, result[0])
        DownLoadImg(url, dirname)


def DownLoadThread(pagestart, pageend):
    for i in range(pagestart, pageend):
        index = i + 1
        print(u'正在下载---------第%d页' % index)
        dirname = "./download/ooxx"
        htmlfile = "%d.html" % index
        url = 'http://jandan.net/ooxx/page-%d#comments' % index
        status = GetHtml(url, dirname + "/" + htmlfile)
        StartDownLoad(htmlfile, dirname, index)
