#include "common.h"



#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    bool isPalindrome(string s) {
        
        int ssize = s.size();

        string new_s = "";

        for(int i = 0; i < ssize; i++){

            char c = s[i];

            if(!isalnum(c)){
                continue;
            }

            char new_c = tolower(c);

            new_s += new_c;

        }


        int newsize = new_s.size();

        bool result = true;
        int front = 0;
        int rear = newsize - 1;

        if(newsize == 0 || newsize == 1){
            return true;
        }

        while(1){

            if(new_s[front] == new_s[rear]){

                if(newsize % 2 == 0){

                    if(front == (rear - 1)){

                        break;
                    }

                } else {

                    if(front == (rear - 2)){

                        break;
                    }

                }

            } else {

                result = false;

                break;
            }

            front += 1;

            rear -= 1;

        }

        return result;

    }
};


int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* line_str = ltrim(rtrim(readline()));

        normalize_line(line_str);

        line_str = ltrim(rtrim(line_str));

        string line_s = line_str;

        Solution s;

        bool result = s.isPalindrome(line_s);

        if(result){

            printf("true\n");
        } else {

            printf("false\n");
        }
        
    }

    return 0;
}