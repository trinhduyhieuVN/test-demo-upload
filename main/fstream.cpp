#include <bits/stdc++.h>
using namespace std;
void insertionSort(vector<int> &v){
     int n = v.size();
     for (int i = 1; i < n; i++){
          int key = v[i];
          int j = i - 1;
          while (j >= 0 && v[j] < key){
                v[j + 1] = v[j];
                j--;
          }
          v[j+1] = key;
     }
}
int main(){
      ifstream fin("main/input.txt");
      ofstream fout("main/output.txt");

      if (!fin.is_open()){
           cerr << "chua doc duoc file" << endl;
           return 1;
      }

     vector<int> v;
     int x;
     vector<int>::iterator it;

     while (fin >> x){
           v.push_back(x);
     }

     int t = 0;
     int n = 3;
     for (auto it = v.begin(); it != v.end(); it++){
           if (*it == n)
                ++t;
     }
     fout << "So lan xuat hien so " << n << " la: " << t << " " << endl;

     sort(v.begin(),v.end(), greater<int>());

     for (auto num : v){
           cout << num << " ";
     }

     insertionSort(v);
     for (auto num : v){
           fout << num << " ";
     }

}