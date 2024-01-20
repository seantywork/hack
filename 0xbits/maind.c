#include "bit/d.h"



int main(){



#ifdef TARGET_STRUCT_COPY_THREAD

    StructCopyThreadInterface();

#endif


#ifdef TARGET_ARR_AS_ARG

    ArrAsArgInterface();

#endif


    return 0;
}