#include "bit/c.h"




int main(){


#ifdef TARGET_MAXAREA


    MaxAreaInterface();


#endif


#ifdef TARGET_LONGEST_PALINDROMIC_SUBSTR

    LongestPalindromicSubstrInterface();


#endif


#ifdef TARGET_REVERSE_INT32

    ReverseInt32Interface();

#endif


    return 0;
}