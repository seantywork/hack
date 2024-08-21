#include "common.h"

#include <bits/stdc++.h>

using namespace std;


class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        
        vector<int> ans;


        int nsize = nums.size();


        int found = 0;

        int front = 0;


        while(1){

            int thisval = 0;

            for(int i = front + 1; i < nsize; i++){

                thisval = nums[front] + nums[i];

                if(thisval == target){

                    ans.push_back(front);
                    ans.push_back(i);

                    found = 1;

                    break;
                }

            }

            if(found == 1){
                break;
            }

            front += 1;

        }

        return ans;
    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* line_raw = readline();

        char* target_raw = readline();

        normalize_line(line_raw);

        int el_len = 0;

        line_raw = ltrim(rtrim(line_raw));

        vector<int> nums;

        int target = parse_int(target_raw);

        char** nums_line = split_string2(line_raw, &el_len);

        for(int i = 0 ; i < el_len; i++){

            nums.push_back(parse_int(nums_line[i]));

        }

        Solution s;

        vector<int> result = s.twoSum(nums, target);

        printf("%d %d\n", result[0], result[1]);
    }

    return 0;
}