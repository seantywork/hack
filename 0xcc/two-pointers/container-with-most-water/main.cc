#include "common.h"

#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    int maxArea(vector<int>& height) {
        
        int top = 0;
        int front = 0;
        int rear = height.size() - 1;

        while(front != rear){

            int h_front = height[front];
            int h_rear = height[rear];
            int distance = rear - front;

            int effctive_height = 0;

            if(h_front > h_rear){

                effctive_height = h_rear;

                rear -= 1;

            } else {

                effctive_height = h_front;

                front += 1;
            }

            int this_result = effctive_height * distance;

            if(this_result > top){
                top = this_result;
            }

        }

        return top;

    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* line_raw = readline();

        normalize_line(line_raw);

        line_raw = ltrim(rtrim(line_raw));

        int arr_len = 0;
        char** line_arr = split_string2(line_raw, &arr_len);

        vector<int> a;

        for(int i = 0; i < arr_len; i++){

            a.push_back(parse_int(line_arr[i]));

        }

        Solution s;

        int result = s.maxArea(a);

        printf("%d\n", result);

        
    }

    return 0;
}