import mysql.connector  

import secrets


def DbExec(query, args):

    ret = {"DB_MSG":'',"DATA":[]}

    cnx = mysql.connector.connect(user='seantywork', password='letsshareitwiththewholeuniverse',
                              host='frankdb',
                              port=3306,
                              database='frank')

    try:
        cursor = cnx.cursor(dictionary=True)
        cursor.execute(query, args)
        result = cursor.fetchall()
        ret["DB_MSG"] = 'DONE'
        ret["DATA"] = result
        cnx.commit()

    except Exception as e:
  
        result = []
        ret["DB_MSG"] = str(e)
        ret["DATA"] = result

    finally:
        cnx.close()

    return ret




def RegisterSessionByEmail(sid, email):

    q = 'SELECT email FROM his_onlyfriends WHERE email = %s'

    a = [email]

    rec = DbExec(q,a)

    if len(rec["DATA"]) != 1 :

        return rec["DB_MSG"]


    req_key = secrets.token_hex(16)

    q = 'UPDATE his_onlyfriends SET f_session = %s, p_key = %s WHERE email = %s'

    a = [sid, req_key, email]

    rec = DbExec(q,a)

    if len(rec["DATA"]) != 1 :

        return rec["DB_MSG"]
    

    return "SUCCESS"


def CheckSessionBySID(sid):

    q = 'SELECT f_session FROM his_onlyfriends WHERE f_session = %s'

    a = [sid]

    rec = DbExec(q,a)

    if len(rec["DATA"]) != 1 :

        return 1
    
    f_session = rec["DATA"][0]["f_session"]

    if f_session != sid :

        return 1
    
    return 0

def GetKeyBySID(sid):
    
    q = 'SELECT p_key FROM his_onlyfriends WHERE f_session = %s'

    a = [sid]

    rec = DbExec(q,a)

    if len(rec["DATA"]) != 1 :

        return ''
    
    p_key = rec["DATA"][0]["p_key"]

    
    return p_key