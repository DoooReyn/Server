#题目：两个乒乓球队进行比赛，各出三人。
# 甲队为a,b,c三人，
# 乙队为x,y,z三人。
# 已抽签决定比赛名单。
# 有人向队员打听比赛的名单。
# a说他不和x比，
# c说他不和x,z比，
# 请编程序找出三队赛手的名单。
#!/usr/bin/python
# -*- coding: UTF-8 -*-

import itertools

A = ["a", "b", "c"]
B = ["x", "y", "z"]
team = []  # 存储比赛名单
rankB = [list(each) for each in itertools.permutations(B)]  # 将对手的全部组合方式列出来

while True:
    flag = 0
    team = list(zip(A, B))  # 匹配选手
    print(team)
    for each in team:
        if (("a" in each) and ("x" in each)) or (("c" in each) and (("x" in each) or ("z" in each))):  # 判断是否符合对阵要求
            flag = 1  # 如不符合则打个标记
            break
    if flag:
        B = rankB.pop()  # 改变一下对手位置
    else:
        break

for v1, v2 in team:
    print("%s 对阵 %s" % (v1, v2))

