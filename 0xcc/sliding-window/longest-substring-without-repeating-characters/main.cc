#include "common.h"

#include <bits/stdc++.h>

using namespace std;


class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        
        int toplen = 0;

        int front = 0;
        int rear = 0;

        int maxlen = s.size();

        if(maxlen == 0){
            return 0;
        }

        vector<char> this_vec;

        while(1){
            
            int move_front = 0;

            char this_c = s[rear];

            auto found = find(this_vec.begin(), this_vec.end(), this_c);

            if(found != this_vec.end()){

                move_front = 1;

            }

            while(move_front == 1){

                front += 1;

                this_vec.erase(this_vec.begin());

                if(front == rear){
                    move_front == 0;
                    break;
                }

                found = find(this_vec.begin(), this_vec.end(), this_c);            

                if(found == this_vec.end()){

                    move_front = 0;

                }
            }

            this_vec.push_back(this_c);

            int this_size = this_vec.size();

            if(this_size > toplen){
                toplen = this_size;
            }

            rear += 1;

            if(rear == maxlen){
                break;
            }

        }

        return toplen;

    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* line_raw = readline();

        normalize_line(line_raw);

        line_raw = ltrim(rtrim(line_raw));


        string a;

        a = line_raw;

        Solution s;

        int result = s.lengthOfLongestSubstring(a);

        printf("%d\n", result);
        
    }

    return 0;
}