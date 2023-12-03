import re
import datetime
import pandas as pd


def populate_iterator_by_col(total_col):


    begin = 65 

    end = 90

    ret_iter = []

    head = ''

    head_ptr = begin

    ptr = begin

    for i in range(total_col):

        addr = head + chr(ptr) 

        ret_iter.append(addr)

        ptr += 1

        if ptr > end :

            ptr = begin 

            head = chr(head_ptr)

            head_ptr += 1       


    return ret_iter


def calculate_col_address_by_col(total_col):


    begin = 65 

    end = 90

    ret_iter = []

    head = ''

    head_ptr = begin

    ptr = begin

    for i in range(total_col):

        addr = head + chr(ptr)

        ret_iter.append(addr)

        ptr += 1

        if ptr > end :

            ptr = begin 

            head = chr(head_ptr)

            head_ptr += 1     


    return ret_iter[-1]



def get_sheets_metadata(service,sheet_id):

    sheet_metadata = service.spreadsheets().get(spreadsheetId=sheet_id).execute()
    sheets = sheet_metadata.get('sheets', '')

    return sheets

def get_sheet(service, sheet_id, target_sheet, sheet_range):

    target_sheet += sheet_range

    sheet = service.spreadsheets()
    result_input = sheet.values().get(spreadsheetId=sheet_id,
                                range=target_sheet).execute()
    values_input = result_input.get('values', [])
    
    return values_input

def update_sheet_per_cell(service, sheet_id, target_sheet, sheet_range, write_val):


    update_target = target_sheet + sheet_range

    sheet = service.spreadsheets()

    values = [[write_val]]

    value_range = {
        "majorDimension" : 'ROWS',
        "values": values
    }

    sheet.values().update(
        spreadsheetId=sheet_id,
        valueInputOption='USER_ENTERED',
        range=update_target,
        body = value_range

    ).execute()



def update_batch(service, sheet_id, batch_update_range):


    service.spreadsheets().values().batchUpdate(
        spreadsheetId=sheet_id, 
        body=batch_update_range
    ).execute()



def populate_local_frame(sheet_col_iterator, values_input):

    df = pd.DataFrame(values_input, columns=sheet_col_iterator)

    return df



def find_match_pattern_by_col(match_pattern, arr):

    try:
        
        el = arr.at[int(match_pattern)]

        if el == '' or el == None:

            raise Exception

        match_parsed = el.replace(' ','').replace('\n','').split('~')[0].split('/')


        if len(match_parsed) != 2 :

            raise Exception

        match_parsed[0] = int(match_parsed[0])

        match_parsed[1] = int(match_parsed[1])        

        if match_parsed[0] > 12 or match_parsed[0] < 1:

            raise Exception
        

        if match_parsed[1] > 31 or match_parsed [1] < 1:

            raise Exception

        return match_parsed


    except:

        return []



    
def check_if_within_date_range(month_now, day_now, month_match, day_match):

    check = False

    if month_now < month_match:

        check = False

    elif month_now == month_match and day_now < day_match:

        check = False

    elif month_now == month_match and day_now >= day_match:

        check = True

    elif month_now > month_match:

        check = True

    return check



def beat_o_per_col(service, sheet_id, target_sheet, sheet_col_iterator, local_frame, match_pattern):

    now = datetime.datetime.now()    

    month_now = now.month

    day_now = now.day


    for i in range(len(sheet_col_iterator)):

        col_lable = sheet_col_iterator[i]

        col_df = local_frame[col_lable]

        match_val = find_match_pattern_by_col(match_pattern, col_df)

        if len(match_val) == 0:

            continue


        if not check_if_within_date_range(month_now, day_now, match_val[0], match_val[1]):

            continue


        for j in range(len(col_df)):

            if col_df.at[j] == 'O' or col_df.at[j] == 'o':

                target_range = '!' + col_lable + str(j+1)

                print('***** found beatable O')
                print('***** > '+ target_sheet)
                print('***** > '+ target_range)

                update_sheet_per_cell(service, sheet_id, target_sheet, target_range, 'X')

                print('***** > updated: ' + target_sheet + target_range)


def collect_o_per_col(service, sheet_id, target_sheet, sheet_col_iterator, local_frame, match_pattern):

    now = datetime.datetime.now()    

    month_now = now.month

    day_now = now.day

    ret_container = []

    for i in range(len(sheet_col_iterator)):

        col_lable = sheet_col_iterator[i]

        col_df = local_frame[col_lable]

        match_val = find_match_pattern_by_col(match_pattern, col_df)

        if len(match_val) == 0:

            continue


        if not check_if_within_date_range(month_now, day_now, match_val[0], match_val[1]):

            continue


        for j in range(len(col_df)):

            if col_df.at[j] == 'O' or col_df.at[j] == 'o':

                target_range = '!' + col_lable + str(j+1)

                print('***** found beatable O')
                print('***** > '+ target_sheet)
                print('***** > '+ target_range)

                print('***** > collected: ' + target_sheet + target_range)

                collection = {
                    "range": target_sheet + target_range,
                    "values": [['X']]
                }

                ret_container.append(collection)

    return ret_container

                




def beat_every_single_o_by_sheet(service, sheet_id, sheet_name, sheet_range, sheet_col_iterator, match_pattern):


    
    target_range = sheet_range

    values_input = get_sheet(service, sheet_id, sheet_name, target_range)

    local_frame = populate_local_frame(sheet_col_iterator, values_input)

    
    beat_o_per_col(service, sheet_id, sheet_name, sheet_col_iterator, local_frame, match_pattern)


    return

def collect_beatable_o_by_sheet(service, sheet_id, sheet_name, sheet_range, sheet_col_iterator, match_pattern):
    
    target_range = sheet_range

    values_input = get_sheet(service, sheet_id, sheet_name, target_range)

    local_frame = populate_local_frame(sheet_col_iterator, values_input)


    return collect_o_per_col(service, sheet_id, sheet_name, sheet_col_iterator, local_frame, match_pattern)
    


# def beat_every_single_o_by_row(service, sheet_id, sheet_name, sheet_range, match_pattern)



if __name__ == '__main__':

    #print(calculate_col_address_by_col(375))

    #print(populate_iterator_by_col(375))

    print('main')