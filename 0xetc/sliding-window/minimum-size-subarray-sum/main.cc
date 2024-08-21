#include "common.h"

#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    int minSubArrayLen(int target, vector<int>& nums) {

        int front = 0;
        int rear = 0;

        int sum = 0;

        int min_len = INT_MAX;

        int hit = 0;

        while(front < nums.size()){

            sum += nums[front];

            while(sum >= target){

                hit = 1;

                min_len = min(min_len, front - rear + 1);

                sum -= nums[rear];

                rear += 1;

                if(rear > front){
                    break;
                }

            }

            front += 1;

        }        

        if(hit != 1){
            return 0;
        } else {
            return min_len;
        }

    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        int target = parse_int(readline());

        char* tmp = readline();

        normalize_line(tmp);

        char* tmp_raw = ltrim(rtrim(tmp));

        int arr_len = 0;
        char** arr_ch = split_string2(tmp_raw, &arr_len);

        vector<int> arr;

        for(int i=0;i<arr_len;i++){

            arr.push_back(parse_int(arr_ch[i]));
        }

        Solution s;

        int result = s.minSubArrayLen(target, arr);
        
        printf("%d\n", result);
    }

    return 0;
}