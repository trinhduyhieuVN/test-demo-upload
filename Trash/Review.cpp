#include <iostream>
using namespace std;

int main() {
     cout<<"nhap n: ";
     int n;
     cin>>n;
     int *p = &n;
     cout<<"Gia tri cua n la:"<< n <<endl;
     cout<<"gia tri cua con tro p la:"<< p <<endl;
     cout<<"Dia chi cua n la:"<< &n <<endl;
     cout<<"Gia tri cua n qua con tro p la:"<< *p <<endl;
}