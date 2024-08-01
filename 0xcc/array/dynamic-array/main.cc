#include "common.h"

#include <bits/stdc++.h>

using namespace std;



/*
 * Complete the 'dynamicArray' function below.
 *
 * The function is expected to return an INTEGER_ARRAY.
 * The function accepts following parameters:
 *  1. INTEGER n
 *  2. 2D_INTEGER_ARRAY queries
 */

vector<int> dynamicArray(int n, vector<vector<int>> queries) {

    vector<vector<int>> arr;

    vector<int> type2ans;

    int lastAnswer = 0;

    int q_size = queries.size();

    for(int i = 0; i < n; i ++){

        vector<int> el;

        arr.push_back(el);
    }

    for(int i = 0; i < q_size; i ++){

        if(queries[i][0] == 1){

            int one_idx = ((queries[i][1] ^ lastAnswer) % n); 

            arr[one_idx].push_back(queries[i][2]);

        } else {

            int two_idx = ((queries[i][1] ^ lastAnswer) % n);

            int arr_idx_size = arr[two_idx].size();

            int two_idx2 = (queries[i][2] %  arr_idx_size); 

            lastAnswer = arr[two_idx][two_idx2];

            type2ans.push_back(lastAnswer);

        }

    }

    return type2ans;

}

int main()
{
    FILE* fout = fopen(getenv("OUTPUT_PATH"), "w");

    char** first_multiple_input = split_string(rtrim(readline()));


    int n = parse_int(first_multiple_input[0]);

    int q = parse_int(first_multiple_input[1]);

    vector<vector<int>> queries;

    for (int i = 0; i < q; i++) {
        
        vector<int> query;

        char** queries_row_temp =  split_string(rtrim(readline()));

        for (int j = 0; j < 3; j++) {

            int queries_row_item = parse_int(queries_row_temp[j]);

            query.push_back(queries_row_item);
        }


        queries.push_back(query);

    }

    vector<int> result = dynamicArray(n, queries);

    int res_size = result.size();

    for (int i = 0; i < res_size; i++) {

        fprintf(fout, "%d", result[i]);

        if (i != result.size() - 1) {
            fprintf(fout, "\n");
        }
    }

    fprintf(fout, "\n");

    fclose(fout);

    return 0;
}

