#include <bits/stdc++.h>

using namespace std;


int string_to_ascii(string letter)
{
    char x = letter.at(0);

    return int(x);    
}

string ascii_to_string(int ascii){

    char ch = ascii;

    string s(1, ch);

    return s;
}


vector<int> code_generator(string letters){

    vector<int> retv;

    int tmp_buff = 0;

    for(int i=0;i<letters.size();i++){


        tmp_buff = string_to_ascii(letters[i]);

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


class Solution {
public:
    string modifyString(string s) {
        
        string ret = "";

        size_t str_len = s.length();


        for (int i = 0; i < str_len; i ++){

            


        }

        size_t found = s.find("?");




        return ret;

    }
};