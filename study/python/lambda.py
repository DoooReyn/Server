

from functools import reduce

# 定义函数（普通方式）
def func(arg):
    return arg + 1

# 执行函数
result = func(123)
print(result)

my_lambda = lambda arg : arg + 1

result = my_lambda(123)
print(result)

li = [11, 22, 33]

new_list = map(lambda a: a + 100, li)
print(list(new_list))

new_list = filter(lambda arg: arg >= 22, li)
print(list(new_list))

result = reduce(lambda x, y: x + y, li)
print(result)