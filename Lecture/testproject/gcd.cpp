#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

int gcd_recursive(int m, int n) {
    if(m == 0) {
        return n;
    }
    if(n==0) {
        return m;
    }
    return gcd_recursive(m, n%m);
}

int main(int argc, char* const argv[]) {
    int m, n;
    istringstream iss;

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <integer m> <integer n>"
             << endl;
        return 1;
    }
    iss.str(argv[1]);
    if ( !(iss >> m) ) {
        cerr << "Error: The first argument is not a valid integer."
             << endl;
        return 1;
    }
    iss.clear(); // clear the error code
    iss.str(argv[2]);
    if ( !(iss >> n) ) {
        cerr << "Error: The second argument is not a valid integer."
             << endl;
        return 1;
    }

    cout << "Recursive gcd: " << gcd_recursive(m, n)<< endl;
    return 0;
}