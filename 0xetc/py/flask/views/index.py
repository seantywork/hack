from flask import render_template, session, redirect, request as request_flask
from oauthlib.oauth2 import WebApplicationClient
import requests
import secrets
import json


from models import index as models_index

import os
from dotenv import load_dotenv, find_dotenv



load_dotenv(find_dotenv())



def SessionAuth(sid):

    flag = 1
    
    ret_bool = False

    flag = models_index.CheckSessionBySID(sid)

    if flag == 0 :

        ret_bool = True
        

    return ret_bool



def Index():

    auth = 0

    if not session.get('SID') :

        return render_template('index.html', AUTH = auth)
    

    sid = session.get('SID')

    if not SessionAuth(sid) :

        return render_template('index.html', AUTH = auth)


    req_key = models_index.GetKeyBySID(sid)

    if req_key == '':

        return render_template('index.html', AUTH = auth)

    auth = 1

    return render_template('index.html', AUTH = auth, REQ_KEY = req_key)


def AuthorizePubKey():

    ret_val = {}

    if not session.get('SID') :

        return ret_val

    sid = session.get('SID')

    if not SessionAuth(sid) :

        return ret_val

    return ret_val


def HealthCheck():

    ret_val = {}

    if not session.get('SID') :

        return ret_val
    
    sid = session.get('SID')

    if not SessionAuth(sid) :

        return ret_val


    ret_val["health"] = 'fine'

    return ret_val



def get_google_provider_cfg():

    GOOGLE_DISCOVERY_URL = os.environ.get("GOOGLE_DISCOVERY_URL")

    return requests.get(GOOGLE_DISCOVERY_URL).json()


def OauthLogin():


    google_provider_cfg = get_google_provider_cfg()
    authorization_endpoint = google_provider_cfg["authorization_endpoint"]

    GOOGLE_CLIENT_ID = os.environ.get("GOOGLE_CLIENT_ID")

    OAUTH_REDIRECT_URI = os.environ.get("OAUTH_REDIRECT_URI")
    
    client = WebApplicationClient(GOOGLE_CLIENT_ID)

    sid = secrets.token_hex(16)

    session["SID"] = sid

    request_uri = client.prepare_request_uri(
        authorization_endpoint,
        redirect_uri=OAUTH_REDIRECT_URI,
        scope=["openid", "email", "profile"],
    )


    return redirect(request_uri)





def OauthLoginCallback():

    if not session.get('SID') :

        return redirect('/')

    sid = session.get('SID')

    code = request_flask.args.get("code")

    GOOGLE_CLIENT_ID = os.environ.get("GOOGLE_CLIENT_ID")
    GOOGLE_CLIENT_SECRET = os.environ.get("GOOGLE_CLIENT_SECRET")
    client = WebApplicationClient(GOOGLE_CLIENT_ID)

    google_provider_cfg = get_google_provider_cfg()
    token_endpoint = google_provider_cfg["token_endpoint"]


    req_url =  request_flask.url

    req_base_url = request_flask.base_url


    if os.environ.get("DEBUG") != 'True':

        req_url = req_url.replace('http','https')

        req_base_url = req_base_url.replace('http','https')


    token_url, headers, body = client.prepare_token_request(
        token_endpoint,
        authorization_response=req_url,
        redirect_url=req_base_url,
        code=code
    )

    token_response = requests.post(
        token_url,
        headers=headers,
        data=body,
        auth=(GOOGLE_CLIENT_ID, GOOGLE_CLIENT_SECRET),
    )

    
    client.parse_request_body_response(json.dumps(token_response.json()))

    userinfo_endpoint = google_provider_cfg["userinfo_endpoint"]
    uri, headers, body = client.add_token(userinfo_endpoint)
    userinfo_response = requests.get(uri, headers=headers, data=body)

    users_email = userinfo_response.json()["email"]

    sig = models_index.RegisterSessionByEmail(sid, users_email)

    print(sig)

    return redirect('/')