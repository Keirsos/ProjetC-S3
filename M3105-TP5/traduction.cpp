#include <iostream>
using namespace std;

int main() {
    int i;
    int j;
    int k;
    int l;
    int m;
    int u;

    j = 0;
    m = 0;
    l = 0;
    k = 1;
    i = 1;
    if(i > 14) {
        m = 1;
    }
    else if(i > 10) {
        m = 2;
    }
    else if(i > 5) {
        m = 3;
    }
    else {
        m = 0;
    }
    while(i <= 20) {
        for(u = 1 ; u <= 2 ; u = u + 1) {
            if(i > 18) {
                cout << i << endl;
            }
        }
        i = i + 1;
    }
    cout << "normal" << endl;
    if(!(j == 5)) {
        do
            j = j + 1;
        while(!(j == 5));
    }
    for(k = 1 ; k <= 5 ; k = k + 1) {
        l = l + k;
    }
    for( ; k <= 5 ; ) {
        l = l + k;
        k = k + 1;
    }
    cout << "k = " << k << endl;
    cin >> k;
    cout << "k = " << k << endl;
    return 0;
}
