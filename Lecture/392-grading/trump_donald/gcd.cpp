#include <iostream>
#include <sstream>

/**
 * Author: Donald Trump
 * Pledge: I pledge my honor I have abided by the Stevens Honor Code
 */

using namespace std;

int gcd_iter(int m, int n){

    while ( n != 0 ) {
        int f = m % n;
        m = n;
        n = f;
    }
    return m;
}

int gcd_rec(int m, int n) {

    if (n == 0) {
        return m;
    } else {
        int f = m % n;
        m = n;
        n = f;
        return gcd_rec(m, n);
    }

}
int main(int argc, char* argv[]) {
    int m, n;
    istringstream iss;

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <integer m> <integer n>" << endl;
        return 1;
    }

    iss.str(argv[1]);
    if (!(iss >> m)) {
        cerr << "Error: The first argument is not a valid integer." << endl;
        return 1;
    }

    iss.clear();
    iss.str(argv[2]);
    if (!(iss >> n)) {
        cerr << "Error: The second argument is not a valid integer." << endl;
        return 1;
    }

    cout << "Iterative: gcd(" << m << ", " << n << ") = " << gcd_iter(m, n) << endl;
    cout << "Recursive: gcd(" << m << ", " << n << ") = " << gcd_rec(m, n) << endl;

    return 0;
}
