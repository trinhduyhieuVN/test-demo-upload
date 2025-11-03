#include <bits/stdc++.h>
using namespace std;

int main() {
     #ifndef ONLINE_JUDGE
     freopen("main/input.txt","r",stdin);
     freopen("main/output.txt","w",stdout);
     #endif
     
     vector <int> v;
     int word;  
     vector<int>::iterator it;

     while (cin >> word){
         v.push_back(word);
     }
     
     for (it = v.begin(); it != v.end(); it++) {
         cout << *it << " ";
     }

     cout<< "\n";

     v.pop_back();

     v.insert(v.begin()+3,123); //them vao vi tri thu 4
     
     cout<<endl;
     v.erase(v.begin()+2); //xoa vi tri thu 3

     for (int i = v.size() - 1; i >= 0; i--){
         cout << v[i] << " ";
     }

     cout << "\n";

     reverse(v.begin(),v.end());

     for (int nums : v){
         cout << nums << " ";
     }

     cout << endl;

     v.insert(v.begin()+2,36);

     for (auto it = v.rbegin(); it != v.rend(); it++){
         cout << *it << " ";
     }

     auto it_min = min_element(v.begin(),v.end());
     auto it_max = max_element(v.begin(),v.end());
     
     cout << endl;
     cout << *it_min << " " << *it_max;

     int n;
     cout << "Nhập số n để kiểm tra n xuất hiện bao nhiêu lần trong vector:" << endl;
     cin >> n;

     int t = 0;

     for (int num : v){
         if (n == num)
             t += 1;      
     }

     cout << t ;
}