
import  tkinter
import  tkinter.font as tkFont

root = tkinter.Tk()

root.geometry('800x400+300+300')
root.title('爬虫程序')

ft = tkFont.Font(size=15)
l_url = tkinter.Label(text="地址:", font=ft)
l_url.place(x=40,y=40)

t_url = tkinter.Text(height=1,width=60,font=ft)
t_url.place(x=95,y=40)

root.mainloop()