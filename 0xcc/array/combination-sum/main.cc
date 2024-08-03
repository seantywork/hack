#include "common.h"

#include <bits/stdc++.h>


using namespace std;

class Solution {
public:

vector<vector<int>> combinationSum(vector<int>& candidates, int target) {





}
};



int main(){

    char* T_str = ltrim(rtrim(readline()));
	
    int T = parse_int(T_str);
	
    for(int i = 0 ; i < T; i++){

        vector<int> candidate_v;

        int target_v;

        char* candidate_raw = readline();

        char* target_raw = ltrim(rtrim(readline()));

        normalize_line(candidate_raw);

        char* candidate_good = ltrim(rtrim(candidate_raw));

        int candidate_len = 0;

        char** candidate = split_string2(candidate_good, &candidate_len);

        for(int i = 0 ; i < candidate_len; i ++){
      
            int newc = parse_int(candidate[i]);

            candidate_v.push_back(newc);
    
        }

        target_v = parse_int(target_raw);

        Solution s;

        vector<vector<int>> result = s.combinationSum(candidate_v, target_v);

        int result_size = result.size();

        for(int i = 0 ; i < result_size; i++){
    
            int result_size2 = result[i].size();

            for(int j = 0; j < result_size2; j++){
      
                printf("%d ", result[i][j]);
      
            }
      
            printf("|");
    
        }
        printf("\n");
	
    }


    return 0;

}
