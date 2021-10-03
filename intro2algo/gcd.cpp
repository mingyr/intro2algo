#include <tchar.h>
#include <assert.h>
#include <iostream>

using namespace std;

/*
int gcd(int a, int b)
{
	if (a > b)
		return a % b ? gcd(a - b, b): b;
	else
		return b % a ? gcd(a, b - a): a;
}
*/

int gcd(int no1, int no2)
{
	return no2 ? gcd(no2, no1 % no2) : no1;
}

void gcd_test(int argc, TCHAR* argv[])
{
	assert(argc == 3);

	int a = _ttoi(argv[1]);
	int b = _ttoi(argv[2]);

	assert(a > 0);
	assert(b > 0);

	cout << "The greatest common divisor of " << a << " and " << b << " is " << gcd(a, b) << endl;
}