#include "common.h"

#include <bits/stdc++.h>

using namespace std;

/*
 * Complete the 'hourglassSum' function below.
 *
 * The function is expected to return an INTEGER.
 * The function accepts 2D_INTEGER_ARRAY arr as parameter.
 */

int hourglassSum(vector<vector<int>> arr) {

    int init = 0;

    int top = 0;


    for(int i = 0; i < 4; i ++){

        for(int j = 0 ; j < 4; j++){


            int sum = 0;


            sum += arr[i][j];
            sum += arr[i][j + 1];
            sum += arr[i][j + 2];

            sum += arr[i + 1][j + 1];

            sum += arr[i + 2][j];
            sum += arr[i + 2][j + 1];
            sum += arr[i + 2][j + 2];

            if(init == 0){

                init = 1;

                top = sum;

            } else {

                if(sum > top){

                    top = sum;

                }
            }


        }
    }


    return top;

}

int main()
{
    FILE* fout = fopen(getenv("OUTPUT_PATH"), "w");

    vector<vector<int>> arr;

    for(int i = 0; i < 6; i ++){

        char** arr_temp = split_string(rtrim(readline()));

        vector<int> row;

        for(int j = 0 ; j < 6 ; j++){

            int el = parse_int(arr_temp[j]);

            row.push_back(el);

        }

        arr.push_back(row);

    }


    int result = hourglassSum(arr);

    fprintf(fout, "%d\n", result);

    fclose(fout);

    return 0;
}

string ltrim(const string &str) {
    string s(str);

    s.erase(
        s.begin(),
        find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace)))
    );

    return s;
}

string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end()
    );

    return s;
}

vector<string> split(const string &str) {
    vector<string> tokens;

    string::size_type start = 0;
    string::size_type end = 0;

    while ((end = str.find(" ", start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));

        start = end + 1;
    }

    tokens.push_back(str.substr(start));

    return tokens;
}
