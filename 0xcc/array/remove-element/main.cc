#include "common.h"

#include <bits/stdc++.h>

using namespace std;


class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        
        int nsize = nums.size();

        vector<int> newv;

        int ans = 0;

        for(int i = 0 ; i < nsize; i ++){

            if(nums[i] != val){

                nums[ans] = nums[i]; 

                ans += 1;
            }


        }

        return ans;


    }
};


int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* lineraw = readline();

        int val = parse_int(ltrim(rtrim(readline())));

        normalize_line(lineraw);

        lineraw = ltrim(rtrim(lineraw));

        int ellen = 0;
        
        char** rows = split_string2(lineraw, &ellen);

        vector<int> nums;

        for(int i = 0 ; i < ellen; i++){

            nums.push_back(parse_int(rows[i]));

        }

        Solution s;

        int ans = s.removeElement(nums, val);

        printf("ans: %d\n > ", ans);


        for(int i = 0; i < ans; i ++){

            printf("%d ", nums[i]);

        }
        
        printf("\n");

    }

    return 0;
}