#include "bit/d.h"



int ExpansionWidthCounter(char* s, int start, int end, int total_len){


    int counter = 1;


    while(start >= 0 && end < total_len){


        char at_start = s[start];

        char at_end = s[end];


        if (at_start != at_end){

            break;

        }


        start -= 1;
        end += 1;
    }
    

    counter = end - start -1;

    return counter;

}


void PollMaxTowardsRight(char* s, int* at, int* longest){


    int total_len = strlen(s);

    for(int i = 0; i < total_len; i ++){

        int start = i;

        int odd_end = i;

        int even_end = i + 1;

        int tmp_ret1 = ExpansionWidthCounter(s, start, odd_end, total_len);

        int tmp_ret2 = ExpansionWidthCounter(s, start, even_end, total_len);

        int max_ret;

        if(tmp_ret1 > tmp_ret2){

            max_ret = tmp_ret1;

        }else{

            max_ret = tmp_ret2;

        }

        if(max_ret > *longest){

            if(tmp_ret1 > tmp_ret2){

                *at = i - tmp_ret1 / 2;
            
            } else{

                *at = i - tmp_ret2 / 2 + 1;

            }

            *longest = max_ret;

        }

    }
    




}





char* longestPalindrome(char* s) {

    char* ret;

    int at = 0;

    int longest = 0;


    PollMaxTowardsRight(s, &at, &longest);

    ret = (char*)malloc(( longest + 1 ) * sizeof(char));


    char* from = &s[at];

    memset(ret, 0, ( longest + 1 ) * sizeof(char));

    strncpy(ret, from, longest);

    return ret;

}