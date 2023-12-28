#include <stdio.h>

int main(){

    int T = 0;

    int left = 0;

    int right = 0;

    scanf("%d",&T);
    
    for (int i = 0 ; i<T; i++){
        
        scanf("%d %d",&left, &right);


        printf("read: l:%d r:%d\n",left, right);
    }

    printf("good bye jam!");

    return 0;
}