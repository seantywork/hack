#include "bit/c.h"




int main(){


#ifdef TARGET_MAXAREA


    MaxAreaInterface();


#endif


#ifdef TARGET_LONGEST_PALINDROMIC_SUBSTR

    LongestPalindromicSubstrInterface();


#endif



    return 0;
}