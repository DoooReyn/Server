#!/usr/bin/python
# -*- coding: UTF-8 -*-

import requests
from bs4 import BeautifulSoup


headers = {'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36'}
r = requests.get('https://www.qiushibaike.com/imgrank/', headers = headers)
content = r.text
soup = BeautifulSoup(content, 'lxml')
divs = soup.find_all(class_ = 'article block untagged mb15 typs_hot')

for div in divs:
    joke = div.span.get_text()
    print(joke)
    print('------')
