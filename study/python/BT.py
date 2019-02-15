import requests
import re
import os

def GetHtml(url, proxy=None):

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

    #print(r.content)
    return r.text

def GetHtmlUrl(url, proxy=None):

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

    #print(r.content)
    return r.url


def GetListUrl(text):
    reobj = re.compile("<a target=\"_blank\" href=\"(.*?)\" class=\".*?\">(.*?)</a>")
    temp = reobj.findall(text)
    list = []
    for item in temp:
        if item[1].find("BT") == -1:
            continue

        list.append(item)
    return list


def DownLoadFile(url, name):
    if url is None:
        print("url is none")
        return

    r = requests.get(url)
    with open(name, 'wb') as f:
        f.write(r.content)

    print(u'file download success:%s' % name)

    return


def GetBTList(list):
    for item in list:
        url = item[0]
        name = item[1]
        #print(url,name)

        text = GetHtml(url)
        reobj = re.search("<a href=\"(.*?)\" class=\"ajaxdialog\" ajaxdialog=\".*>(.*?)</a>",text)
        #print(reobj.group(1),reobj.group(2))
        name = reobj.group(2)

        if os.path.exists(name):
            print(u'%s file exists'% name)
            continue


        addrtext = GetHtml(reobj.group(1))
        #print(addrtext)
        addrtext = addrtext.replace('\\','')
        #print(addrtext)
        reobj2 = re.search("<a href=\"(.*?)\" target=\"_blank\"><span class=\"icon icon-download\">", addrtext)

        url = reobj2.group(1)
        addrurl  = GetHtmlUrl(url);

        #print(addrurl)
        DownLoadFile(addrurl,name)





text = GetHtml("http://www.btbtt77.com/")
list = GetListUrl(text)
GetBTList(list)
