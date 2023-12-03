import pandas as pd

from dotenv import load_dotenv,find_dotenv
import pickle
import os 


from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow,Flow
from google.auth.transport.requests import Request

import beater_mod

import sys

BASE_DIR = sys.argv[1] + '/'

load_dotenv(find_dotenv())



SHEET_ID = os.environ.get('SHEET_ID')
SHEETS_IGNORE = os.environ.get('SHEETS_IGNORE')
SHEET_RANGE = os.environ.get('SHEET_RANGE')
CRED_PATH = os.environ.get('CRED_PATH')
MATCH_PATTERN_CASE = os.environ.get('MATCH_PATTERN_CASE')
MATCH_PATTERN = os.environ.get('MATCH_PATTERN')

SCOPES = ['https://www.googleapis.com/auth/spreadsheets']

sheets_ignore_list = SHEETS_IGNORE.split(',')

CRED_PATH = BASE_DIR + CRED_PATH

creds = None

sheet_counter = 0

running_error = 0

if os.path.exists(BASE_DIR+'token.pickle'):
    with open(BASE_DIR + 'token.pickle', 'rb') as token:
        creds = pickle.load(token)
if not creds or not creds.valid:
    if creds and creds.expired and creds.refresh_token:
        creds.refresh(Request())
    else:
        flow = InstalledAppFlow.from_client_secrets_file(
            CRED_PATH, SCOPES) 
        creds = flow.run_local_server(port=0)
    with open(BASE_DIR + 'token.pickle', 'wb') as token:
        pickle.dump(creds, token)


service = build('sheets', 'v4', credentials=creds)

sheets_metadata = beater_mod.get_sheets_metadata(service, SHEET_ID)

batch_update_range = {
    "valueInputOption": "RAW",
    "data": [

    ]
}

for i in range(len(sheets_metadata)):


    sheet_name = sheets_metadata[i].get("properties", {}).get("title", "")

    sheet_col_idx = 0

    sheet_col_iterator = []

    if sheet_name in sheets_ignore_list:
        continue

    if MATCH_PATTERN_CASE == 'COL':

        sheet_col_idx = sheets_metadata[i].get("properties", {}).get("gridProperties", {}).get("columnCount",0)

        sheet_col_iterator = beater_mod.populate_iterator_by_col(sheet_col_idx)


    at_str = str(sheet_counter)

    print('AT: '+at_str+': ********** '+sheet_name)

    try:

        if MATCH_PATTERN_CASE == 'COL':

            #beater_mod.beat_every_single_o_by_sheet(service, SHEET_ID, sheet_name, SHEET_RANGE, sheet_col_iterator, MATCH_PATTERN)

            container = beater_mod.collect_beatable_o_by_sheet(service, SHEET_ID, sheet_name, SHEET_RANGE, sheet_col_iterator, MATCH_PATTERN)

            for el in container:

                batch_update_range["data"].append(el)

        else :

            print('AT: '+at_str+': ERROR '+': ROW not supported')

            raise Exception

        
        sheet_counter += 1

    except Exception as e:

        print('AT: '+at_str+': ERROR '+sheet_name)
        print(e)
        
        running_error = 1
        break


if running_error == 1:

    print('!!!!!!!!!! Aborted due to error')

else:

    beater_mod.update_batch(service, SHEET_ID, batch_update_range)

    f = open(BASE_DIR+"_success", "w")
    f.write("SUCCESS")
    f.close()

    print('SUCCESS')
