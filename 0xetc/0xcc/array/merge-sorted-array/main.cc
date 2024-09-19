#include "common.h"

#include <bits/stdc++.h>


using namespace std;

class Solution {
public:
    void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {

        if(m == 0 && n == 0){
            return;
        }

        if(n == 0){
            return;
        }

        int idx = m;

        for(int i = 0; i < n; i++){

            nums1[idx] = nums2[i];

            idx += 1;
        }

        sort(nums1.begin(), nums1.end());

    }
};


int main()
{



    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* nums1_str = ltrim(rtrim(readline()));

        normalize_line(nums1_str);

        int nums1_len = parse_int(readline());

        char* nums2_str = ltrim(rtrim(readline()));

        normalize_line(nums2_str);

        int nums2_len = parse_int(readline());

        int nums1_els = 0;
        int nums2_els = 0;

        vector<int> nums1;
        vector<int> nums2;

        char** nums1_arr = split_string2(nums1_str, &nums1_els);
        char** nums2_arr = split_string2(nums2_str, &nums2_els);


        for(int i = 0; i < nums1_els; i ++){

            nums1.push_back(parse_int(nums1_arr[i]));

        }


        for(int i = 0; i < nums2_els; i ++){
            nums2.push_back(parse_int(nums2_arr[i]));
            
        }


        Solution s;

        s.merge(nums1, nums1_len, nums2, nums2_len);


        for(int i = 0 ; i < nums1.size(); i ++){

            printf("%d ", nums1[i]);

        }

        printf("\n");

    }

    return 0;
}

