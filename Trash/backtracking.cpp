#include <bits/stdc++.h>
using namespace std;
int arr[4];
bool check[4];

void backtrack(int step){
     if (step > 4)
     {
        for (int i = 0; i < 4; i++) 
        {
            cout << arr[i] << " ";
        }
        cout<<endl;
        return;
     }
     for (int i = 0; i < 4; i++)
     {
        if (check[i] == 0)
        {
            arr[i] = step;
            check[i] = 1;
            backtrack(step+1); 
            check[i] = 0;
        }
        
     }
}

int main(){
     backtrack(1);
}