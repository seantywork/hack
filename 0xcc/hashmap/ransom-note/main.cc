#include "common.h"

#include <bits/stdc++.h>

using namespace std;


class Solution {
public:
    bool canConstruct(string ransomNote, string magazine) {
        
        map<char, int> mag_map;

        bool result = true;

        int ran_size = ransomNote.size();

        int mag_size = magazine.size();


        for(int i = 0; i < mag_size ; i ++){

            char mkey = magazine[i];

            if(mag_map.find(mkey) == mag_map.end()){

                mag_map.insert({mkey, 1});

            } else {

                mag_map[mkey] += 1;

            }

        }

        for(int i = 0; i < ran_size; i++){

            char rkey = ransomNote[i];

            if(mag_map.find(rkey) == mag_map.end()){

                result = false;

                break;                
            } 

            int count = mag_map[rkey] - 1;

            if(count < 0){

                result = false;

                break;
            }

            mag_map[rkey] = count;

        }


        return result;
    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* line_raw1 = readline();

        char* line_raw2 = readline();

        normalize_line(line_raw1);
        normalize_line(line_raw2);

        line_raw1 = ltrim(rtrim(line_raw1));
        line_raw2 = ltrim(rtrim(line_raw2));

        string a = line_raw1;
        string b = line_raw2;

        Solution s;

        bool result = s.canConstruct(a, b);

        if(result){
            printf("true\n");
        } else {
            printf("false\n");
        }
        
    }

    return 0;
}