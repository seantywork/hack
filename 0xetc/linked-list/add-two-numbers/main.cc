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

        vector<ListNode*> make_ans;

        int carry = 0;        

        while(1){

            int l1end = 0;
            int l2end = 0;

            if(l1 != nullptr){

                number_one = l1->val;

               // printf("n1: %d ", number_one);


                l1 = l1->next;

            } else {

                l1end = 1;
                number_one = 0;
            
            }

            if(l2 != nullptr){

                number_two = l2->val;


               // printf("n2: %d\n", number_two);


                l2 = l2->next;



            } else {

                l2end = 1;
                number_two = 0;
            
            }

            if(carry == 0 && l1end == 1 && l2end == 1){

                break;
            }

            int sum = carry + number_one + number_two;

            carry = sum / 10;

            int thisval = sum % 10;

            ListNode *ln = new ListNode(thisval);

            make_ans.push_back(ln);

        }

        int mk_len = make_ans.size();

        for(int i = 0; i < mk_len; i++){

            ListNode* a;

            a = make_ans[i];

            if(i != mk_len - 1){

                a->next = make_ans[i + 1];
            }

            if(i == 0){

                ans = a;
            }

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
        

        vector<ListNode*> ln1;
        vector<ListNode*> ln2;

        ListNode* l1;
        ListNode* l2;

        int valint;


        for(int i = 0; i < ll1_len; i++){

            valint = parse_int(ll1[i]);

            ListNode* a = new ListNode(valint);
            
            ln1.push_back(a);

        }

        for(int i = 0; i < ll2_len; i++){

            valint = parse_int(ll2[i]);

            ListNode* a = new ListNode(valint);

            ln2.push_back(a);

        }

        for(int i = 0; i < ll1_len; i++){

            ListNode* a;

            a = ln1[i];

            if(i != ll1_len -1){

                a->next = ln1[i + 1];
            
            }

            if(i == 0){

                l1 = a;
            }


        }

        for(int i = 0; i < ll2_len; i++){

            ListNode* a;

            a = ln2[i];

            if(i != ll2_len -1){

                a->next = ln2[i + 1];
            
            }


            if(i == 0){

                l2 = a;
            }


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