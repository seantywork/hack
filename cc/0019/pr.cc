#ifndef _0019_DEF
#define _0019_DEF
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

#endif

class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        
    }
};