#include "common.h"

#include <bits/stdc++.h>

using namespace std;

class Solution {
public:

    string top;
    int found ;

    Solution(){

        this->top = "";
        this->found = 0;
    }

    void throwToTop(string* s){

        if(s->size() > this->top.size()){

            this->top = *s;
            this->found = s->size();
        }

    }

    void longerTheWinner(string* s){

        int ssize = s->size();

        if(ssize == 1){
            
            throwToTop(s);

            return;

        }

        int i = 0;

        int found = 0;

        while(1){

            int fidx = i;
            int ridx = ssize - i - 1;

            char front = (*s)[fidx];
            char rear = (*s)[ridx];

            if(ssize % 2 == 0){

                if(fidx > ridx){
                    found = 1;
                    break;
                }

            } else {

                if(fidx == ridx){
                    found = 1;
                    break;
                }

            }

            if(front != rear){

                break;

            }

            i += 1;

        }

        if(found == 1){

            throwToTop(s);

            return;
        }


    }


    string longestPalindrome(string s) {
    
        int ssize = s.size();
        int epoch_size = ssize;


        if(ssize == 1){

            return s;
        }


        while(1){

            int end = ssize - epoch_size + 1;
            
            for(int i = 0; i < end; i++){

                string teststr = "";

                int epoch_end = i + epoch_size;

                for(int j = i; j < epoch_end; j++){

                    teststr += s[j];
                }

                this->longerTheWinner(&teststr);
                
                if(this->found != 0){
                    return this->top;
                }
            }

            epoch_size -= 1;
        }

        return this->top;
    }
};


int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* lineraw = readline();

        normalize_line(lineraw);

        lineraw = ltrim(rtrim(lineraw));


        string q = lineraw;

        Solution s;

        string result = s.longestPalindrome(q);

        printf("%s\n", result.c_str());

    }

    return 0;
}