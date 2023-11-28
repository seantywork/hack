#include <iostream>
#include "something.h"
using namespace std;

Something::Something(string in_name){
	this->name = in_name; 
}

void Something::speak(){
	cout << "the name of something is " << this->name << endl;
}

