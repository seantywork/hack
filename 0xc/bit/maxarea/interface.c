#include "bit/c.h"



#define MAX_HEIGHT_N 10 * 10 * 10 * 10 * 10

void MaxAreaInterface(){

    int T = 0;

    scanf("%d\n", &T);

    for (int i = 0 ; i < T; i++){

        int n = 0;

        int el = 0;

        char line[MAX_HEIGHT_N * 2] = {0};

        int tmp[MAX_HEIGHT_N] = {0};

        char* ptk;

        char* delim = " ";

        int ret = 0;

        fgets(line, MAX_HEIGHT_N * 2, stdin);

        ptk = strtok(line, delim);

        while(ptk != NULL){

            el = atoi(ptk);

            tmp[n] = el;

            n += 1;    

		    ptk = strtok(NULL, delim);
                    
        }


        int* height = (int*)malloc(n * sizeof(int));

        memset(height, 0 , n * sizeof(int));

        memcpy(height, tmp, n * sizeof(int));

        ret = maxArea(height, n);

        printf("answer: %d\n", ret);

        free(height);
        
    }


}