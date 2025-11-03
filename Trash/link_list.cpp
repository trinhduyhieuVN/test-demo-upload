#include <bits/stdc++.h>
using namespace std;

struct Node {
     int data;
     Node* next;
};

typedef struct Node* node;

node newNode(int x) {
     node temp = new Node();
     temp->data = x;
     temp->next = NULL;
     return temp;
};

bool checkEmpty(node a) {
     return a == NULL ? true : false;
}

int sizeOfNode(node a){
     int count = 0;
     while (a != NULL) {
            count++;
            a = a->next; //gan dia chi node tiep theo cho node hien tai
     }
     return count;
}

void insertFirst(node &a, int x) {
     node tmp = newNode(x);
     if (a == NULL) {
         a = tmp;
     } else {
         tmp ->next = a;
         a = tmp;
     }
}
void insertLast(node &a, int x) {
     node tmp = newNode(x);
     if (a == NULL) {
           a = tmp;
      } else {
           node curr = a;
           while (curr->next != NULL) {
                curr = curr->next;
           }
           curr->next = tmp;
     }
}

void insertMiddle(node &a, int x, int pos) {
     node tmp = newNode(x);
     if (a == NULL) {
           a = tmp;
      } else {
           node curr = a;
           for (int i = 1; i < pos - 1 && curr->next != NULL; i++) {
                curr = curr->next;
           }
           tmp->next = curr->next;
           curr->next = tmp;
     }
}

void deleteFirst(node &a) {
     if (a == NULL) return;
     a = a->next;
}

int main() {
    int n;
    cin >> n;
    node head = NULL;
    insertFirst(head, 15);
    insertFirst(head, 10);
    insertFirst(head, 18);
    insertLast(head,20);
    insertLast(head,30);
    insertMiddle(head,17,n);
    deleteFirst(head);
    for (node curr = head; curr != NULL; curr = curr->next) {
        cout << curr->data << " ";
    }
    cout << endl;
    cout << "Size of linked list: " << sizeOfNode(head) << endl;
    return 0;
}