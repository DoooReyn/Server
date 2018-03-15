#!/usr/bin/python
# -*- coding: UTF-8 -*-


#!/usr/bin/python
# -*- coding: UTF-8 -*-
import requests

response  = requests.get("https://www.baidu.com")
print(type(response))
print(response.status_code)
print(type(response.text))
print(response.text)
print(response.cookies)
print(response.content)
print(response.content.decode("utf-8"))

# response =requests.get("http://www.baidu.com")
# response.encoding="utf-8"
# print(response.text)
#
# requests.post("http://httpbin.org/post")
# requests.put("http://httpbin.org/put")
# requests.delete("http://httpbin.org/delete")
# requests.head("http://httpbin.org/get")
# requests.options("http://httpbin.org/get")

# response = requests.get("http://httpbin.org/get?name=zhaofan&age=23")
# print(response.text)
#
# data = {
#     "name":"zhaofan",
#     "age":22
# }
# response = requests.get("http://httpbin.org/get",params=data)
# print(response.url)
# print(response.text)


# import json
#
# response = requests.get("http://httpbin.org/get")
# print(type(response.text))
# print(response.json())
# print(json.loads(response.text))
# print(type(response.json()))
#
# headers = {
#     "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36"
# }
# response = requests.get("https://www.zhihu.com", headers=headers)
#
# print(response.text)

# import requests
#
# data = {
#     "name":"zhaofan",
#     "age":23
# }
# response = requests.post("http://httpbin.org/post",data=data)
# print(response.text)


# response = requests.get("http://www.baidu.com")
# print(response.cookies)
#
# for key,value in response.cookies.items():
#     print(key+"="+value)
#
# s = requests.Session()
# s.get("http://httpbin.org/cookies/set/number/123456")
# response = s.get("http://httpbin.org/cookies")
# print(response.text)

# from requests.packages import urllib3
# urllib3.disable_warnings()
#
# response = requests.get("https:/www.12306.cn",verify=False)
# print(response.status_code)


