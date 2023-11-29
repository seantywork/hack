#include <bits/stdc++.h>

using namespace std;


#define UPPER_BOUNDARY 122
#define LOWER_BOUNDARY 97

int string_to_ascii(string letter, int i)
{
    char x = letter.at(i);

    return int(x);    
}

string ascii_to_string(int ascii){

    char ch = ascii;

    string s(1, ch);

    return s;
}

int forward_aware_next_letter_is(vector<int>* codes, int letter_v, int current_v, int tail_v){

    int one_step_ahead = 0;

    if (current_v + 1 <= tail_v){

        one_step_ahead = (*codes)[current_v + 1];

    }

    if (one_step_ahead == 63){

        if (letter_v + 1 > UPPER_BOUNDARY){

            letter_v = LOWER_BOUNDARY;

        } else {
            letter_v += 1;
        }


        return letter_v;
    }

    if (letter_v + 1 > UPPER_BOUNDARY){

        letter_v = LOWER_BOUNDARY;

        if(one_step_ahead == letter_v){

            letter_v += 1;

        }

    } else {

        letter_v += 1;

        if(one_step_ahead == letter_v){

            letter_v += 1;

            if(letter_v > UPPER_BOUNDARY){
                letter_v = LOWER_BOUNDARY;
            }

        } 
    }

    return letter_v;

}


vector<int> code_generator(string letters){

    vector<int> retv;

    int tmp_buff = 0;

    for(int i=0;i<letters.size();i++){


        tmp_buff = string_to_ascii(letters, i);

        retv.push_back(tmp_buff);

    }

    return retv;

}

string code_inverter(vector<int> codes){

    string rets;

    
    for(int i=0;i<codes.size();i++){

        rets += ascii_to_string(codes[i]);

    }

    return rets;
}



void forward_tick_up_letter_until_tail(vector<int>* codes, int* letter, int *current, int *tail){

    int letter_v = (*letter);

    int current_v = (*current);

    int tail_v = (*tail);

    if((*codes)[current_v] == 63){
      
        if (current_v == tail_v){

            (*letter) = forward_aware_next_letter_is(codes, letter_v, current_v, tail_v);            

            (*codes)[current_v] = (*letter);

            return;
        } else {

            (*letter) = forward_aware_next_letter_is(codes, letter_v, current_v, tail_v); 

            (*codes)[current_v] = (*letter);

            (*current) += 1;

            forward_tick_up_letter_until_tail(codes, letter, current, tail);

        }
    } else {

        if (current_v == tail_v){            

            return;
        } else {

            letter_v = (*codes)[current_v];

            (*letter) = forward_aware_next_letter_is(codes, letter_v, current_v, tail_v);  

            (*current) += 1;

            forward_tick_up_letter_until_tail(codes, letter, current, tail);

        }


    }

}


class Solution {
public:
    string modifyString(string s) {
        
        string ret = "";

        vector<int> codes = code_generator(s);

        int letter = LOWER_BOUNDARY;

        int current = 0;

        int tail = codes.size() - 1;

        forward_tick_up_letter_until_tail(&codes, &letter, &current, &tail);

        ret = code_inverter(codes);

        return ret;

    }
};