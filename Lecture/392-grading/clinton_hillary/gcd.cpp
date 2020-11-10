/*
 * gcd.cpp
 *
 *  Created on: May 18, 2017
 *      Author: Hillary Clinton
 *      I pledge my honor that I have abided by the Stevens Honor System.
 */
#include <iostream>
#include <sstream>

using namespace std;

/**
 * Iterative solution to finding the Greatest Common Denominator of two numbers
 */
int gcd_iter(int m, int n) {

	while (n != 0) {

		int a = m;
		int b = n;

		m = b;
		n = a % b;
	}

	return m;
}

/**
 * Recursive solution to finding the Greatest Common Denominator between two numbers
 */
int gcd_rec(int m, int n) {
	if (n == 0) return m;

	else {

		int em = m;
		int en = n;

		m = en;
		n = em % en;

		return gcd_rec(m, n);

	}
}

/**
 * Main method
 */
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
