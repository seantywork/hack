#ifndef GH_BOT_UTILS
#define GH_BOT_UTILS
#include "glob.h"
#include <time.h>
#include <stdlib.h>


void Copy2DSquareForMaxOwnersLen(char target[MAX_OWNERS_LEN][MAX_OWNERS_LEN], char source[MAX_OWNERS_LEN][MAX_OWNERS_LEN]){

    int len = MAX_OWNERS_LEN;

    for (int i = 0; i< len; i++){

        for (int j = 0; j < len;j++){

            target[i][j] = source[i][j];
        }

    }

}

int GetRandNumInRange(int lower, int upper) { 

    int num = (rand() % (upper - lower + 1)) + lower;  

    return num;
} 
 


#endif