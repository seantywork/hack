import time, win32con, win32api, win32gui, ctypes
from pywinauto import clipboard 
from pywinauto import keyboard
from pywinauto import mouse
import pywinauto
import pandas as pd 
from dotenv import load_dotenv, find_dotenv
import os
import re

import meTheGpt

load_dotenv(find_dotenv('./cred/.env'))


TARGET_CHATROOM_NAME = os.environ.get('TARGET_CHATROOM_NAME')


def findAllSub(match, text):

    result = []

    result = [m.start() for m in re.finditer(match, text)]

    return result

def chatStdizer(ttext):

    std_list = []

    lang = ''

    a = ttext.split('\r\n')   

    std_a = [x for x in a if x.find(']') >= 0]

    for i in std_a:

        nm_txt = []

        idx_list = findAllSub(']',i)

        nm_last_idx = idx_list[0]

        txt_start_idx = idx_list[1] + 2

        nm = i[1:nm_last_idx]

        txt = i[txt_start_idx:]



        if txt.find('mpt:') < 0 and txt.find('ㅇㅍㅌ:') < 0:

            continue

        if txt.find('mpt:') == 0 :

            txt = txt.replace('mpt:','')

            lang = 'English'

        if txt.find('ㅇㅍㅌ:') == 0:

            txt = txt.replace('ㅇㅍㅌ:','')

            lang = 'Korean'


        nm_txt.append(nm)
        nm_txt.append(txt)
        nm_txt.append(lang)

        std_list.append(nm_txt)


    df = pd.DataFrame(std_list,columns=['NM','TXT','LANG'])    


    return df




def copyChatRecord(chatroom_name):

    w = win32con
    
    hwndMain = win32gui.FindWindow( None, chatroom_name)
    hwndListControl = win32gui.FindWindowEx(hwndMain, None, "EVA_VH_ListControl_Dblclk", None)



    PostKeyEx(hwndListControl, ord('A'), [w.VK_CONTROL], False)
    time.sleep(0.5)
    PostKeyEx(hwndListControl, ord('C'), [w.VK_CONTROL], False)
    ctext = clipboard.GetData()	

    return ctext


def PostKeyEx(hwnd, key, shift, specialkey):


    PBYTE256 = ctypes.c_ubyte * 256
    _user32 = ctypes.WinDLL("user32")
    GetKeyboardState = _user32.GetKeyboardState
    SetKeyboardState = _user32.SetKeyboardState
    PostMessage = win32api.PostMessage
    SendMessage = win32gui.SendMessage
    FindWindow = win32gui.FindWindow
    IsWindow = win32gui.IsWindow
    GetCurrentThreadId = win32api.GetCurrentThreadId
    GetWindowThreadProcessId = _user32.GetWindowThreadProcessId
    AttachThreadInput = _user32.AttachThreadInput

    MapVirtualKeyA = _user32.MapVirtualKeyA
    MapVirtualKeyW = _user32.MapVirtualKeyW

    MakeLong = win32api.MAKELONG
    w = win32con

    if IsWindow(hwnd):

        ThreadId = GetWindowThreadProcessId(hwnd, None)

        lparam = MakeLong(0, MapVirtualKeyA(key, 0))
        msg_down = w.WM_KEYDOWN
        msg_up = w.WM_KEYUP

        if specialkey:
            lparam = lparam | 0x1000000

        if len(shift) > 0: 
            pKeyBuffers = PBYTE256()
            pKeyBuffers_old = PBYTE256()

            SendMessage(hwnd, w.WM_ACTIVATE, w.WA_ACTIVE, 0)
            AttachThreadInput(GetCurrentThreadId(), ThreadId, True)
            GetKeyboardState(ctypes.byref(pKeyBuffers_old))

            for modkey in shift:
                if modkey == w.VK_MENU:
                    lparam = lparam | 0x20000000
                    msg_down = w.WM_SYSKEYDOWN
                    msg_up = w.WM_SYSKEYUP
                pKeyBuffers[modkey] |= 128

            SetKeyboardState(ctypes.byref(pKeyBuffers))
            time.sleep(0.01)
            PostMessage(hwnd, msg_down, key, lparam)
            time.sleep(0.01)
            PostMessage(hwnd, msg_up, key, lparam | 0xC0000000)
            time.sleep(0.01)
            SetKeyboardState(ctypes.byref(pKeyBuffers_old))
            time.sleep(0.01)
            AttachThreadInput(GetCurrentThreadId(), ThreadId, False)

        else:  
            SendMessage(hwnd, msg_down, key, lparam)
            SendMessage(hwnd, msg_up, key, lparam | 0xC0000000)





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

    w = win32con
    
    PostKeyEx(whndChild, ord(' '), [w.VK_CONTROL], False)
    time.sleep(0.5)

    win32api.SendMessage(whndChild, win32con.WM_SETTEXT, 0, text)
    win32api.PostMessage(whndChild, win32con.WM_KEYDOWN, win32con.VK_RETURN, 0)
    time.sleep(0.5)
    win32api.PostMessage(whndChild, win32con.WM_KEYUP, win32con.VK_RETURN, 0)


def chatHandleSender(df):


    for i in range(len(df)):

        req_text =  df.iloc[i, 1]

        lang = df.iloc[i,2]

        resp_text = meTheGpt.getFakeResponse(req_text, lang)

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


