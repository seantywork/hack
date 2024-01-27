#include "bit/d.h"



void LongestPalindromicSubstrInterface(){

    int T = 0;

    scanf("%d\n", &T);


    for (int i = 0 ; i < T; i++){

        char* ret;

        char line[MAX_LONGEST_PALINDROMIC_SUBSTR_LENGTH];

        memset(line, 0, sizeof(char) * MAX_LONGEST_PALINDROMIC_SUBSTR_LENGTH);

        fgets(line, MAX_LONGEST_PALINDROMIC_SUBSTR_LENGTH, stdin);


        for(int i = 0 ; i < MAX_LONGEST_PALINDROMIC_SUBSTR_LENGTH; i ++){

            if(line[i] == '\n'){
                line[i] = 0;
            }
        }

        ret = longestPalindrome(line);

        printf("answer: %s\n", ret);

        free(ret);

    }


}