
import urllib.parse
import urllib.request
import urllib.error
import socket


# get 调用
#response = urllib.request.urlopen('http://www.baidu.com')
# print(response.read().decode('utf-8'))

# post 调用
#data = bytes(urllib.parse.urlencode({'word': 'hello'}), encoding='utf8')
#print(data)

#try:
#    response = urllib.request.urlopen('http://httpbin.org/post', data=data, timeout=1)
#except urllib.error.URLError as e:
#    if isinstance(e.reason, socket.timeout):
#        print('TIME OUT')

#request2 = urllib.request.Request('https://python.org')
#response2 = urllib.request.urlopen(request2)
#print(response2.read().decode('utf-8'))

from urllib import request, parse

url = 'http://httpbin.org/post'
headers = {
    'User-Agent': 'Mozilla/4.0 (compatible; MSIE 5.5; Windows NT)',
    'Host': 'httpbin.org'
}
dict = {
    'name': 'zhaofan'
}

data = bytes(parse.urlencode(dict), encoding='utf8')
print(data)
req = request.Request(url=url, data=data, headers=headers, method='POST')
response = request.urlopen(req)
print(response.read().decode('utf-8'))

from urllib.parse import urlparse
result = urlparse("http://www.baidu.com/index.html;user?id=5#comment")
print(result)
print(result.netloc)
print(result.hostname)


from urllib.parse import urlunparse
data = ['http','www.baidu.com','index.html','user','a=123','commit']
print(urlunparse(data))
