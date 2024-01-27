#include "bit/d.h"

int GetMaxAreaPerHead(int* height, int head, int tail){

    int max_area = 0;

    while(1){

        if(head == tail){
            break;
        }

        int tmp_maxarea = 0;

        int x  = tail - head;

        int y = 0;

        if(height[head] > height[tail]){
            y = height[tail];

            tail -= 1;

        } else {
            y = height[head];

            head += 1;
        }

        tmp_maxarea = x * y;

        if(tmp_maxarea > max_area){
            max_area = tmp_maxarea;
        }


    }


    return max_area;
}


int maxArea(int* height, int heightSize) {

    int answer = 0;

    int head = 0;

    int tail = heightSize - 1;

    int result = GetMaxAreaPerHead(height, head, tail);

    answer = result;

    return answer;
}


