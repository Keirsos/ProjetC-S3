#include <iostream>
using namespace std;

int main() {
    int var;

    var = ((1 + 2) * 2);
    if((((1 == 0) || (1 == 1)) && (0 == 1))) {
        cout << "test1" << endl;
    }
    else {
        cout << "!test1" << endl;
    }
    if(((((1 + 2) * 2) != 5) && ((((1 + 6) - 4) >= 2) || (0 == 1)))) {
        cout << "test2" << endl;
    }
    else {
        cout << "!test2" << endl;
    }
    cout << var << endl;
    return 0;
}
