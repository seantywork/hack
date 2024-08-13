#include "common.h"

#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    bool isPalindrome(int x) {

        bool result;

        char x_str[16] = {0};

        sprintf(x_str, "%d", x);

        int xsize = 0;

        for(int i = 0; i < 16; i++){

            if(x_str[i] == '\0'){

                xsize = i;

                break;
            }

        }



        if(xsize == 1){

            return true;
        }

        if(x_str[0] == '-'){
            return false;
        }

        int i = 0;


        if(xsize % 2 == 0){

            while(1){

                int f_idx = i;
                int r_idx = xsize - i - 1;

                if(f_idx > r_idx){
                    result = true;
                    break;
                }

                char front = x_str[f_idx];
                char rear = x_str[r_idx]; 
                

                if(front != rear){
                    return false;
                }

                i += 1;

            }

        } else {

            while(1){

                int f_idx = i;
                int r_idx = xsize - i - 1;

                if(f_idx == r_idx){
                    result = true;
                    break;
                }

                char front = x_str[f_idx];
                char rear = x_str[r_idx]; 

                if(front != rear){
                    return false;
                }

                i += 1;

            }

        }

        return result;
    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* line_raw = readline();

        int x = parse_int(line_raw);

        Solution s;

        bool result = s.isPalindrome(x);

        if(result){

            printf("true\n");

        } else {

            printf("false\n");
        }


        
    }

    return 0;
}