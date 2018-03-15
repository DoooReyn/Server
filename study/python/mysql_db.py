#!/usr/bin/python
# -*- coding: UTF-8 -*-
# mysql 查询封装


import MySQLdb
import Log
import os
import string


class DBConnect(object):
    host = ""
    user = ""
    passwd = ""
    dbname = ""
    handle = {}

    def __init__(self, host, user, passwd, dbname):
        self.host = host
        self.user = user
        self.passwd = passwd
        self.dbname = dbname
        self.handle = MySQLdb.connect(
            host, user, passwd, dbname, charset="utf8")

    def Print(self):
        print "host:%s user:%s passwd:%s dbname:%s" % (
            self.host, self.user, self.passwd, self.dbname)


def Exec(db, strsql, isfile=True):

    Log.INFO(strsql)
    # 使用cursor()方法获取操作游标
    cursor = db.handle.cursor(cursorclass=MySQLdb.cursors.DictCursor)

    if isfile is True:
        cmd = "mysql -h%s -u%s -p%s %s -e\" source %s\"" % (
            db.host, db.user, db.passwd, db.dbname, strsql)
        # print cmd
        os.system(cmd)
        return
    else:
        # 执行语句
        cursor.execute(strsql)
        # 获取结果
        results = cursor.fetchall()

    # 游标关闭
    cursor.close()

    return results


def GetDBInfo():
    HOST = "192.168.0.179"
    USER = "root"
    PASSWD = "123456"
    DBNAME = "shouhuzhemen_gm"
    DBTYEP = 4

    # 打开数据库连接
    Conn = DBConnect(HOST, USER, PASSWD, DBNAME)

    commond = "select * from server_mapping where dbtype = " + str(DBTYEP)
    dataset = Exec(Conn, commond, False)

    mapConn = {}
    for row in range(len(dataset)):
        serid = dataset[row]["id"]
        host = dataset[row]["dbip"]
        dbname = dataset[row]["dbname"]
        username = dataset[row]["username"]
        userpwd = dataset[row]["userpwd"]
        dbconn = DBConnect(host, username, userpwd, dbname)
        mapConn[serid] = dbconn

    # for (k,v) in mapConn.items():
    #     v.Print()
    Conn.handle.close()
    return mapConn


def ListFile(dir, filetype):
    filelist = []
    pathdir = os.listdir(dir)
    for file in pathdir:
        pos = string.find(file, filetype)
        if pos != -1:
            filelist.append(file)

    return filelist


def Main():
    FileList = ListFile("./", ".sql")
    MapDBConn = GetDBInfo()

    for k, v in MapDBConn.items():
        for i in range(len(FileList)):
            strsql = FileList[i]
            Exec(v, strsql, True)
            # dataset = Exec(v, strsql, True)
            # for row in range(len(dataset)):
            #     print dataset[row]
        Log.DEBUG("%s  %s success" % (v.host, v.dbname))


Main()
