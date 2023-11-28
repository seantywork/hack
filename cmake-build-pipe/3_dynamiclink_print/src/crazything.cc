#include <iostream>
#include "crazything.h"
using namespace std;

Crazything::Crazything(string in_status){
	this->status = in_status;
}

void Crazything::yell(){
	cout << "...AND I'M SUFFERING FROM " << this->status << " !!!!!!!!!"<< endl;
}

