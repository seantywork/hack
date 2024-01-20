#include "bit/c.h"

int GetMaxAreaPerHead(int* height, int head, int tail){

    int max_area = 0;

    int head_y = height[head];

    for(; tail > head; tail--){
                
        int tail_y = height[tail];

        int distance = tail - head;

        int top_y = 0;

        if(head_y <= tail_y){

            top_y = head_y;

        } else {

            top_y = tail_y;
        }

        int tmp = distance * top_y;

        if(tmp > max_area){

            max_area = tmp;
        }

    }


    return max_area;
}


int maxArea(int* height, int heightSize) {

    int answer = 0;

    int tail = heightSize - 1;

    int head_end = tail - 1;

    int max_area = 0;

    for(int head = 0; head <= head_end; head ++){

        int result = GetMaxAreaPerHead(height, head, tail);

        if(result > max_area){

            max_area = result;
        }
    }

    answer = max_area;

    return answer;
}


