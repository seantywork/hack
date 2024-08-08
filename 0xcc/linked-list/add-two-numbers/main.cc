#include "common.h"

#include <bits/stdc++.h>

using namespace std;

class ListNode {
public:
    int val;
    ListNode *next;

    ListNode() {
        
        this->val = 0;
        this->next = nullptr;


    }

    ListNode(int x){

        this->val = x;
        this->next = nullptr;
    }

    ListNode(int x, ListNode *next) {

        this->val = x;
        this->next = next;

    }
};

class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        
        int number_one = 0;
        int number_two = 0;

        ListNode* ans = nullptr;

        ListNode* ans_ptr = nullptr;

        int carry = 0;        

        while(1){

            if(l1 != nullptr){

                number_one = l1->val;

                printf("n1: %d ", number_one);


                l1 = l1->next;

            } else {

                number_one = 0;
            
            }

            if(l2 != nullptr){

                number_two = l2->val;


                printf("n2: %d\n", number_two);


                l2 = l2->next;



            } else {

                number_two = 0;
            
            }

            if(carry == 0 && number_one == 0 && number_two == 0){

                break;
            }

            int sum = carry + number_one + number_two;

            carry = sum / 10;

            int thisval = sum % 10;

            ListNode *ln = new ListNode(thisval);

            ans_ptr = ln;

            if(ans == nullptr){

                ans = ans_ptr;
            }

            ans_ptr = ans_ptr->next;

        }

        return ans;

    }
};

int main()
{

    int T = parse_int(readline());

    for(int t = 0 ; t < T; t ++){

        char* line_raw1 = readline();
        char* line_raw2 = readline();

        normalize_line(line_raw1);
        normalize_line(line_raw2);

        line_raw1 = ltrim(rtrim(line_raw1));
        line_raw2 = ltrim(rtrim(line_raw2));

        int ll1_len = 0;
        int ll2_len = 0;

        char** ll1 = split_string2(line_raw1, &ll1_len);
        char** ll2 = split_string2(line_raw2, &ll2_len);
        
        
        ListNode* l1_ptr = nullptr;

        ListNode* l1 = nullptr;

        ListNode* l2_ptr = nullptr;    

        ListNode* l2 = l2_ptr;
        
        int valint = 0;

        for(int i = 0; i < ll1_len; i++){

            valint = parse_int(ll1[i]);

            ListNode* new_ln = new ListNode(valint);

            l1_ptr = new_ln;

            if(l1 == nullptr){
                l1 = l1_ptr;
            }

            l1_ptr = l1_ptr->next;

        }

        for(int i = 0; i < ll2_len; i++){

            valint = parse_int(ll2[i]);

            ListNode* new_ln = new ListNode(valint);

            l2_ptr = new_ln;

            if(l2 == nullptr){
                l2 = l2_ptr;
            }

            l2_ptr = l2_ptr->next;

        }

        Solution s;

        ListNode* ans = s.addTwoNumbers(l1, l2);
        
        while(ans != nullptr){

            printf("%d ", ans->val);

            ans = ans->next;

        }
        printf("\n");
    }

    return 0;
}