import re
import datetime



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

def update_sheet(service, sheet_id, target_sheet, sheet_range, write_val):


    target_sheet += sheet_range

    sheet = service.spreadsheets()




def find_match_pattern_by_col(match_pattern, arr):

    hit = 0

    for el in arr:

        x = re.search(match_pattern, el[0])

        if x:
            hit = 1
            break

    if hit == 1 :

        return el[0][x.start():x.end()]
    
    else:

        return ''


def dd():

    now = datetime.datetime.now()
    
    m = now.month

    d = now.day

    print(m)
    print(d)

    return 0
    
def check_if_within_date_range(match_val, month_now, day_now):

    check = False

    match_parsed = match_val.replace(' ','').replace('\n','').split('~')[0].split('/')

    match_month = int(match_parsed[0])

    match_day = int(match_parsed[1])

    if month_now < match_month:

        check = False

    elif month_now == match_month and day_now < match_day:

        check = False

    elif month_now == match_month and day_now >= match_day:

        check = True

    elif month_now > match_month:

        check = True

    return check


def beat_o_batch(service, sheet_id, target_sheet, base_address, target_arr):

    for i in range(len(target_arr)):

        if target_arr[i] == 'X' or target_arr[i] == 'x':

            row_num = str(i + 1)

            target_address = '!' + base_address + row_num

            update_sheet(service, sheet_id, target_sheet, target_address, 'O')




def beat_every_single_o_by_col(service, sheet_id, sheet_name, sheet_col_iterator, match_pattern):

    now = datetime.datetime.now()    

    month_now = now.month

    day_now = now.day

    for i in range(len(sheet_col_iterator)):

        target_range = '!' + sheet_col_iterator[i] + str(1) + ':' + sheet_col_iterator[i] + str(1000)
        
        values_input = get_sheet(service, sheet_id, sheet_name, target_range)

        match_val = find_match_pattern_by_col(match_pattern, values_input)


        if match_val == '':

            continue


        within_date_range = check_if_within_date_range(match_val, month_now, day_now)
        
        if not within_date_range:

            break
        

        beat_o_batch(service, sheet_id, sheet_name, sheet_col_iterator[i], values_input)


    return
    


# def beat_every_single_o_by_row(service, sheet_id, sheet_name, sheet_range, match_pattern)



if __name__ == '__main__':

    #print(calculate_col_address_by_col(375))

    #print(populate_iterator_by_col(375))

    dd()