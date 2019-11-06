#include <iostream>
using namespace std;

int main() {
    int i;
    int j;
    int var;
    int var2;

    var = 42;
    for(i = 0 ; (i < 10) ; i = (i + 1)) {
        var = (var + 2);
    }
    j = 0;
    var2 = 0;
    for( ; (j <= 5) ; ) {
        var2 = (var2 + j);
        j = (j + 1);
    }
    return 0;
}
