import time, win32con, win32api, win32gui, ctypes
from apscheduler.schedulers.background import BackgroundScheduler
from pywinauto import clipboard 
from pywinauto import keyboard
from pywinauto import mouse
import pandas as pd 
from dotenv import load_dotenv, find_dotenv
import os
import re

import meTheGpt

load_dotenv(find_dotenv('./cred/.env'))


TARGET_CHATROOM_NAME = os.environ.get('TARGET_CHATROOM_NAME')


TARGET_CHATROOM_X = int(os.environ.get('TARGET_CHATROOM_X'))

TARGET_CHATROOM_Y = int(os.environ.get('TARGET_CHATROOM_Y'))


PBYTE256 = ctypes.c_ubyte * 256
_user32 = ctypes.WinDLL("user32")


def findAllSub(match, text):

    result = []

    result = [m.start() for m in re.finditer(match, text)]

    return result

def chatStdizer(ttext):

    std_list = []

    a = ttext.split('\r\n')   

    std_a = [x for x in a if x.find(']') >= 0]

    for i in std_a:

        nm_txt = []

        idx_list = findAllSub(']',i)

        nm_last_idx = idx_list[0]

        txt_start_idx = idx_list[1] + 2

        nm = i[1:nm_last_idx]

        txt = i[txt_start_idx:]



        if txt.find('mpt:') < 0:

            continue

        txt = txt.replace('mpt:','')

        nm_txt.append(nm)
        nm_txt.append(txt)

        std_list.append(nm_txt)


    df = pd.DataFrame(std_list,columns=['NM','TXT'])    


    return df




def copyChatRecord(chatroom_name):


    
    hwndMain = win32gui.FindWindow( None, chatroom_name)
    hwndListControl = win32gui.FindWindowEx(hwndMain, None, "EVA_VH_ListControl_Dblclk", None)
    mouse.click(button='left', coords=(TARGET_CHATROOM_X, TARGET_CHATROOM_Y))
    time.sleep(1)
    keyboard.send_keys('^a^c')
    ctext = clipboard.GetData()

    return ctext




def findChatTarget(chatroom):
    whnd = win32gui.FindWindow(None, "KakaoTalk")
    whnd_OnlineMainView = win32gui.FindWindowEx(whnd, None, "EVA_ChildWindow", None)
    whnd_ContactListView = win32gui.FindWindowEx(whnd_OnlineMainView, None, "EVA_Window", None)
    whnd_ChatRoomListView = win32gui.FindWindowEx(whnd_OnlineMainView, whnd_ContactListView, "EVA_Window", None)
    whnd_MessageBox = win32gui.FindWindowEx( whnd_ChatRoomListView, None, "Edit", None)

    win32api.SendMessage(whnd_MessageBox, win32con.WM_SETTEXT, 0, chatroom)
    time.sleep(1)
    win32api.PostMessage(whnd_MessageBox, win32con.WM_KEYDOWN, win32con.VK_RETURN, 0)
    time.sleep(0.5)
    win32api.PostMessage(whnd_MessageBox, win32con.WM_KEYUP, win32con.VK_RETURN, 0)
    time.sleep(1)



def sendChatMessage(chatroom, text):
    if not isinstance(text, str): text = str(text)
    whndMain = win32gui.FindWindow(None, chatroom)
    whndChild = win32gui.FindWindowEx(whndMain, None, "RICHEDIT50W", None)
    

    win32api.SendMessage(whndChild, win32con.WM_SETTEXT, 0, text)
    win32api.PostMessage(whndChild, win32con.WM_KEYDOWN, win32con.VK_RETURN, 0)
    time.sleep(0.5)
    win32api.PostMessage(whndChild, win32con.WM_KEYUP, win32con.VK_RETURN, 0)


def chatHandleSender(df):


    for i in range(len(df)):

        req_text =  df.iloc[i, 1]

        resp_text = meTheGpt.getFakeResponse(req_text)

        out_text = 'Response to -> '+ df.iloc[i, 0] + ' : ' + resp_text

        sendChatMessage(TARGET_CHATROOM_NAME, out_text)


def chatInit():


    findChatTarget(TARGET_CHATROOM_NAME) 
    ttext = copyChatRecord(TARGET_CHATROOM_NAME)  


    df = chatStdizer(ttext)

    if len(df) == 0 :

        print("nothing to parse")

        return 0, 'N', 'N'

    idx = df.index[-1]

    nm = df.iloc[-1, 0]

    txt = df.iloc[-1, 1]

    return idx, nm, txt


def chatHandle(idx, nm, txt):
    findChatTarget(TARGET_CHATROOM_NAME) 
    ttext = copyChatRecord(TARGET_CHATROOM_NAME)  

    df = chatStdizer(ttext)

    if len(df) == 0:

        print("nothing to parse")

        return 0, 'N', 'N'

    if df.iloc[-1, 0] == nm and df.iloc[-1, 1] == txt:

        print("no previous query")

        idx = df.index[-1]

        nm = df.iloc[-1, 0]

        txt = df.iloc[-1, 1]

        return idx, nm, txt
        

    else:
        
        print("previous query exists")

        df_recent = df.iloc[idx+1 : ]   # recent

        chatHandleSender(df_recent)
        
        idx = df.index[-1]

        nm = df.iloc[-1, 0]

        txt = df.iloc[-1, 1]

        return idx, nm, txt





def main():


    last_idx, last_nm, last_txt = chatInit()  # initial_setting last chat save


    while True:
        print("running.................")
        last_idx, last_nm, last_txt = chatHandle(last_idx, last_nm, last_txt)  
        time.sleep(1)


if __name__ == '__main__':

    main()
