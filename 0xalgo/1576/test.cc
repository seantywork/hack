
#include <bits/stdc++.h>

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

int main(){

    string in_s = "a";

    int out_int = string_to_ascii(in_s);

    cout << "out int: " << out_int << endl;

    string out_s = ascii_to_string(out_int);

    cout << "out s: " << out_s << endl;
    
    return 0;
}