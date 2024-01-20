#include "bit/d.h"



int main(){



#ifdef TARGET_STRUCT_COPY_THREAD

    StructCopyThreadInterface();

#endif


#ifdef TARGET_ARR_AS_ARG

    ArrAsArgInterface();

#endif

#ifdef TARGET_STREAM_TO_2D

    StreamTo2dInterface();

#endif


    return 0;
}