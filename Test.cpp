#include <iostream>
using namespace std;
int main() {
    cout<<"Nhap 1 so tu 1-1000: ";
    int n;
    cin>>n;
    for (int i = 0; i < n; i++) {
        cout << i+1 << endl;
    }
    return 0;
}