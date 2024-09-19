#include "common.h"

#include <bits/stdc++.h>

using namespace std;


class Solution {
public:
    bool isValidSudoku(vector<vector<char>>& board) {

        for(int i = 0 ; i < 9; i++){

            vector<char> x;
            vector<char> y;


            for(int j = 0 ; j < 9; j ++){


                if((i % 3 == 0) && (j % 3 == 0)){

                    vector<char> sub;

                    for(int si = i; si < i + 3; si++){

                        for(int sj = j; sj < j + 3; sj++){

                            char subc = board[si][sj];


                            if(subc == '.'){
                                continue;
                            }


                            if(find(sub.begin(), sub.end(), subc) != sub.end()){
                                return false;

                            } else {
                                
                                sub.push_back(subc);
                            }

                        }


                    }

                }

                char xc = board[i][j];
                char yc = board[j][i];

                if(xc != '.'){
                    if(find(x.begin(), x.end(), xc) != x.end()){

                        return false;
                    } else {
                        x.push_back(xc);
                    }
                }

                if(yc != '.'){

                    if(find(y.begin(), y.end(), yc) != y.end()){

                        return false;

                    } else {

                        y.push_back(yc);
                    }


                }

            }

                        
        }

        return true;
    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        vector<vector<char>> board;


        for(int i = 0; i < 9; i++){

            vector<char> row;

            char* line_raw = readline();
            
            normalize_line(line_raw);

            line_raw = ltrim(rtrim(line_raw));

            int ellen = 0;
            
            char** line = split_string2(line_raw, &ellen);

            for(int i = 0; i < ellen; i++){

                char* c = line[i];

                row.push_back(*c);

            }

            board.push_back(row);

        }


        Solution s;
        
        bool result = s.isValidSudoku(board);

        if(result){

            printf("true\n");
        } else {
            printf("false\n");
        }
    }

    return 0;
}