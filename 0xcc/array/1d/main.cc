#include "common.h"

#include <bits/stdc++.h>


using namespace std;

/*
 * Complete the 'reverseArray' function below.
 *
 * The function is expected to return an INTEGER_ARRAY.
 * The function accepts INTEGER_ARRAY a as parameter.
 */

vector<int> reverseArray(vector<int> a) {

    vector<int> b;

    int a_len = a.size();

    for(int i = a_len - 1 ; i >= 0; i--){

        b.push_back(a[i]);

    }

    return b;

}

int main()
{
    FILE* fout = fopen(getenv("OUTPUT_PATH"), "w");

    int arr_count = parse_int(ltrim(rtrim(readline())));

    char** arr_temp = split_string(rtrim(readline()));

    vector<int> arr(arr_count);

    for (int i = 0; i < arr_count; i++) {
        int arr_item = parse_int(*(arr_temp + i));

        arr[i] = arr_item;
    }

    vector<int> res = reverseArray(arr);

    int res_len = res.size();

    for (int i = 0; i < res_len; i++) {

        fprintf(fout, "%d", res[i]);

        if (i != res_len - 1) {
            fprintf(fout, " ");
        }

    }
    fprintf(fout, "\n");

    fclose(fout);


    return 0;
}
