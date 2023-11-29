#include "include/algo.h"

using namespace std;

int string_to_ascii(string letter)
{
    char x = letter.at(0);

    return int(x);    
}

string ascii_to_string(int ascii){

    char ch = ascii;

    string s(1, ch);

    return s;
}
