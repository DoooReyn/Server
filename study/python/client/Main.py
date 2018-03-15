import socket
import time
import struct


class PackHead(object):
    head = 0x01020304  # 数据包头部
    uid = 0  # 用户id
    messageid = 0  # 消息id
    size = 0  # 消息包长度(不包括头部)
    tail = 0x05060708  # 数据包尾部
    __data = None
    def Serialized(self):
        self.__data = struct.pack('IIIII', self.head, self.uid,self.messageid,self.size,self.tail)
        return self.__data

    def ParseData(self,data):
        self.head, self.uid, self.messageid, self.size, self.tail = struct.unpack('IIIII', data)

    def Size(self):
        if self.__data is None:
            self.Serialized()
        return len(self.__data)


class CmdConnectVerifyRequest(object):
    id = 0
    conntype = 0
    __data = None
    def Serialized(self):
        self.__data = struct.pack('II', self.id, self.conntype)
        return self.__data

    def ParseData(self,data):
        self.id, self.conntype = struct.unpack('II', data)


    def Size(self):
        if self.__data is None:
            self.Serialized()
        return len(self.__data)


class CmdConnectVerifyReturn(object):
    result = 0		    #是否链接上
    src = ""             #源IP 30长度
    __data = None
    def Serialized(self):
        if self.__data is None:
            self.src.ljust(30,'\0')
            self.__data = struct.pack('I30s', self.result, self.src.encode())
        return self.__data

    def ParseData(self,data):
        self.result, src = struct.unpack('I30s', data)
        self.src = bytes.decode(src)


    def Size(self):
        if self.__data is None:
            self.Serialized()
        return len(self.__data)


def Main():
    obj = socket.socket()
    print(obj)
    ret = obj.connect(("127.0.0.1", 3000))
    print(ret)


    sendPack = CmdConnectVerifyRequest()

    sendPack.id = 0
    sendPack.conntype = 1

    data = bytes()

    header = PackHead()
    header.messageid = 1111
    header.size = sendPack.Size()
    header.uid = 0

    data += header.Serialized()
    data += sendPack.Serialized()

    obj.sendall(data)


    print(header.Size())
    print(sendPack.Size())

    ret = obj.recv(1024)
    print(len(ret))

    recvlen = len(ret)
    newHeader = PackHead()
    if recvlen > newHeader.Size():
        newHeader.ParseData(ret[:newHeader.Size()])
        #print(newHeader.head)
        #print(newHeader.uid)
        #print(newHeader.messageid)
        #print(newHeader.size)
        #print(newHeader.tail)
        if newHeader.size + newHeader.Size() <= recvlen :
            ret = ret[newHeader.Size():]
            recvPack = CmdConnectVerifyReturn()

            print(len(ret),newHeader.size)

            recvPack.ParseData(ret[:newHeader.size])
            print(recvPack.result)
            print(recvPack.src)
            recvPack.Serialized()

    while True:
        time.sleep(1)


Main()
