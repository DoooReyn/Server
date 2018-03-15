import requests
import  time

# url_list = [
#     'http://www.baidu.com',
#     'http://www.pythonsite.com',
#     'http://www.cnblogs.com/'
# ]
#
# start = time.time()
# for url in url_list:
#     result = requests.get(url)
#     #print(result.text)
#
# end = time.time()
#
#
# print(end - start)
#
# from concurrent.futures import ThreadPoolExecutor
# def fetch_request(url):
#     result = requests.get(url)
#     #print(result.text)
#
# url_list = [
#     'http://www.baidu.com',
#     'http://www.pythonsite.com',
#     'http://www.cnblogs.com/'
# ]
# pool = ThreadPoolExecutor(10)
#
# start = time.time()
# for url in url_list:
#     #去线程池中获取一个线程，线程去执行fetch_request方法
#     pool.submit(fetch_request,url)
#
# pool.shutdown(True)
# end = time.time()

from concurrent.futures import ThreadPoolExecutor

def fetch_async(url):
    response = requests.get(url)

    return response


def callback(future):
    print(future.result().text)


url_list = [
    'http://www.baidu.com',
    'http://www.bing.com',
    'http://www.cnblogs.com/'
]

pool = ThreadPoolExecutor(5)

for url in url_list:
    v = pool.submit(fetch_async,url)
    #这里调用回调函数
    v.add_done_callback(callback)

pool.shutdown()