/*
 * gcd.cpp
 *
 *  Created on: May 18, 2017
 *      Author: Homer Simpson
 *      I pledge my honor that I have abided by the Stevens Honor System
 */
#include <iostream>
#include <sstream>
#include <cmath>
using namespace std;

/**
 * Computes the GCD of m and n using Euclid's algorithm and an iterative loop.
 */
int iterativeGCD(int m, int n){
	int h = 0;
	while(n != 0){
		h = n;
		n = m%n;
		m = h;
	}
	return m;
}

/**
 * Computes the GCD of m and n using Euclid's algorithm and recursion.
 */
int recursiveGCD(int m, int n){
	if (n == 0){
		return m;
	}
	return recursiveGCD(n, m%n);
}

/**
 * Outputs the GCD of arguments 1 and 2 using both an iterative algorithm and a recursive algorithm.
 */
int main(int argc, char* argv[]){
	int m, n;
	istringstream iss;

	if(argc != 3){
		cerr << "Usage: " << argv[0] << " <integer m> <integer n>" << endl;
		return 1;
	}

	iss.str(argv[1]);
	if(!(iss >> m)){
		cerr << "Error: The first argument is not a valid integer." << endl;
		return 1;
	}
	iss.clear();
	iss.str(argv[2]);
		if(!(iss >> n)){
			cerr << "Error: The second argument is not a valid integer." << endl;
			return 1;
	}
	iss.clear();
	cout << "Iterative: gcd(" << m << ", " << n << ") = " << iterativeGCD(abs(m),abs(n)) << endl;
	cout << "Recursive: gcd(" << m << ", " << n << ") = " << recursiveGCD(abs(m),abs(n)) << endl;
	return 0;
}



