# coding:utf-8
import sys
import MySQLdb
import ctypes
import codecs

STD_INPUT_HANDLE = -10
STD_OUTPUT_HANDLE= -11
STD_ERROR_HANDLE = -12

FOREGROUND_BLACK = 0x0
FOREGROUND_BLUE = 0x01 # text color contains blue.
FOREGROUND_GREEN= 0x02 # text color contains green.
FOREGROUND_RED = 0x04 # text color contains red.
FOREGROUND_YELLOW = 0x06 # text color contains red.
FOREGROUND_INTENSITY = 0x08 # text color is intensified.


class Color:
    ''' See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winprog/winprog/windows_api_reference.asp
    for information on Windows APIs. - www.sharejs.com'''
    std_out_handle = ctypes.windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)

    def set_cmd_color(self, color, handle=std_out_handle):
        """(color) -> bit
        Example: set_cmd_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
        """
        bool = ctypes.windll.kernel32.SetConsoleTextAttribute(handle, color)
        return bool

    def reset_color(self):
        self.set_cmd_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

    def RedPrint(self, print_text):
        self.set_cmd_color(FOREGROUND_RED | FOREGROUND_INTENSITY)
        print print_text
        self.reset_color()

    def GreenPrint(self, print_text):
        self.set_cmd_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
        print print_text
        self.reset_color()

    def BluePrint(self, print_text):
        self.set_cmd_color(FOREGROUND_BLUE | FOREGROUND_INTENSITY)
        print print_text
        self.reset_color()

    def YellowPrint(self, print_text):
        self.set_cmd_color(FOREGROUND_YELLOW | FOREGROUND_INTENSITY)
        print print_text
        self.reset_color()




class FIELD_TYPE_REDFINE(frozenset):

    """A special type of set for which A == x is true if A is a
    DBAPISet and x is a member of that set."""

    def __eq__(self, other):
        if isinstance(other, FIELD_TYPE_REDFINE):
            return not self.difference(other)
        return other in self

STRING = FIELD_TYPE_REDFINE([MySQLdb.FIELD_TYPE.ENUM,
							 MySQLdb.FIELD_TYPE.STRING,
							 MySQLdb.FIELD_TYPE.VAR_STRING,
							 MySQLdb.FIELD_TYPE.DATE,
							 MySQLdb.FIELD_TYPE.NEWDATE,
							 MySQLdb.FIELD_TYPE.BLOB,
							 MySQLdb.FIELD_TYPE.DATETIME]
							   )

NUMBER = FIELD_TYPE_REDFINE([MySQLdb.FIELD_TYPE.DECIMAL,
							 MySQLdb.FIELD_TYPE.INT24,
							 MySQLdb.FIELD_TYPE.LONG,
							 MySQLdb.FIELD_TYPE.TINY,
							 MySQLdb.FIELD_TYPE.YEAR,
							 MySQLdb.FIELD_TYPE.NEWDECIMAL]
							)

LONGLONG = FIELD_TYPE_REDFINE([MySQLdb.FIELD_TYPE.LONGLONG]
							  )

FLOAT = FIELD_TYPE_REDFINE([MySQLdb.FIELD_TYPE.FLOAT,
							MySQLdb.FIELD_TYPE.DOUBLE]
						   )

def check_contain_chinese(text):
    for ch in text:
        if u'\u4e00' <= ch <= u'\u9fff':
            return True
    return False



def InitDB(host,user,password,dbname):
	conn = MySQLdb.connect(host,user,password,dbname,charset="utf8")
	return conn

def Exec(conn,table):
	cursor = conn.cursor()
	sql = "select * from " + table + " limit 1"
	cursor.execute(sql)
	fields = cursor.description
	#print(fields)
	head = []

	colum = []
	for field in fields:
		if STRING == field[1]:
			colum.append('`'+ field[0] +'`')

	if len(colum) > 0:
		strcolum = ",".join(colum)
		sql2 = "select %s from %s" % (strcolum,table)
		#print(sql2)
		cursor2 = conn.cursor()
		cursor2.execute(sql2)
		results2 = cursor2.fetchall()
		for line2 in results2:
			#print(line2)
			for i in range(len(colum)):
				text = line2[i]
				if text is None:
					continue
				if check_contain_chinese(text):
					ContextDic[text] = 1

				#strtext = text.encode('utf8')
				#if strtext.isdigit() or strtext.isalpha():
				#	continue
				#print(str,text,type(str))
				#print text
				#ContextDic[text] = 1




def GetTable():
	argc = len(sys.argv)
	if argc > 1:
		table = sys.argv[1]
	else:
		table = raw_input("table name:")
	return table


def DBTable(conn):
	cursor = conn.cursor()
	sql = "show tables"
	cursor.execute(sql)
	results = cursor.fetchall()

	for table in results:
		#print table[0]
		Exec(conn, table[0])


def ReadTable():
	conn = InitDB(host, user, password, dbname)
	#table = GetTable()
	#Exec(conn, table)
	DBTable(conn)


def WriteFile():


	sql = "insert into s_type_language(`language_0`) values"
	for key in ContextDic:
		sql += "('%s')," % key

	sql=sql.rstrip(",")

	f = codecs.open('test.sql', 'w', 'utf8')
	f.write(sql)
	f.close()

	f = codecs.open('test.txt', 'w', 'utf8')
	for key in ContextDic:
		f.write(key +"\n")
	f.close()


clr = Color()
host = "127.0.0.1"
user = "root"
password = "123456"
dbname = "shouhuzhemen_dyn"

ContextDic = {}
ReadTable()
WriteFile()




