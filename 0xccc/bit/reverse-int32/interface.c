#include "bit/c.h"

void ReverseInt32Interface(){


    int T = 0;

    scanf("%d\n", &T);

    for (int i = 0 ; i < T; i++){


        int x = 0;

        int ret = 0;

        scanf("%d\n", &x);

        ret = reverse(x);

        printf("%d\n", ret);

    }



}