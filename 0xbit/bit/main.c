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


#ifdef TARGET_STRUCT_COPY_THREAD

    StructCopyThreadInterface();

#endif


#ifdef TARGET_ARR_AS_ARG

    ArrAsArgInterface();

#endif

#ifdef TARGET_STREAM_TO_2D

    StreamTo2dInterface();

#endif

#ifdef TARGET_STREAM_TO_2D_COPY

    StreamTo2dCopyInterface();

#endif


#ifdef TARGET_STREAM_TO_2D_STRUCT

    StreamTo2dStructInterface();

#endif

#ifdef TARGET_PUSH_BACK_STRING

    PushBackStringInterface();

#endif


#ifdef TARGET_VECTOR_STRING

    VectorStringInterface();

#endif

    return 0;
}