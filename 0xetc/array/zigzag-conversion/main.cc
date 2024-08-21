#include "common.h"

#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    string convert(string s, int numRows) {
        
        string ans = "";

        if(numRows == 1){
            return s;
        }

        vector<vector<char>> zz_tile;

        for(int i = 0 ; i < numRows; i++){

            vector<char> row;
            zz_tile.push_back(row);

        }

        int ssize = s.size();

        int reached_full = 0;
        int do_switch = 0;

        int i = 0;
        int row_ptr = 0;
        int col_ptr;
        
        char target_c;

        while(1){


            while(reached_full == 0){

                target_c = s[i];

                i += 1;

                zz_tile[row_ptr].push_back(target_c);

                row_ptr += 1;

                if(i == ssize){
                    break;
                }

                if(row_ptr == numRows){
                    row_ptr = 0;
                    reached_full = 1;
                    do_switch = 1;
                    break;
                }

            }

            if(i == ssize){
                break;
            }

            if(do_switch == 1){
                do_switch = 0;
                col_ptr = numRows - 2;
                if(col_ptr == 0){
                    reached_full = 0;
                }
                continue;
            }

            while(reached_full == 1){

                if(row_ptr != col_ptr){

                    target_c = ' ';

                } else {

                    target_c = s[i];

                    i += 1;
                }

                zz_tile[row_ptr].push_back(target_c);

                row_ptr += 1;


                if(i == ssize){
                    break;
                }

                if(row_ptr == numRows){
                    row_ptr = 0;
                    col_ptr -= 1;

                    if(col_ptr < 1){
                        reached_full = 0;
                        break;
                    }
                }
            }

            if(i == ssize){
                break;
            }


        }

        for(int i = 0; i < numRows; i++){

            int row_len = zz_tile[i].size();

            for(int j = 0 ; j < row_len; j++){

                char t = zz_tile[i][j];

                if(t == ' '){
                    continue;
                }

                ans += t;

            }

        }

        return ans;
    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* line_raw1 = readline();

        normalize_line(line_raw1);

        char* line_row = readline();

        line_raw1 = ltrim(rtrim(line_raw1));

        string a = line_raw1;

        int row = parse_int(ltrim(rtrim(line_row)));

        Solution s;

        string ans = s.convert(a, row);

        printf("%s\n", ans.c_str());

        
    }

    return 0;
}