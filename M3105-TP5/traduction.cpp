#include <iostream>
using namespace std;

int main() {
    int i;
    int res1;
    int res2;

    i = 2;
    res1 = 0;
    res2 = 0;
    switch(i) {
        case        1 :
            res1 = 5;
            break;
        case        2 :
            res1 = 10;
            break;
    }
    switch(i) {
        case       10 :
            res2 = 0;
            break;
        case      666 :
            res2 = 1;
            break;

        default :
            res2 = 42;
    }
    return 0;
}
