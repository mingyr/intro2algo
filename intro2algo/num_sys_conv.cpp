#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> dec2bin(int n)
{
	vector<int> bin;
	do
	{
		int least_bit = n & 0x1;
		
		n >>= 1;
		bin.push_back(least_bit);
	} while (n != 0);

	return bin;
}

void dec2bin_test()
{
	int n = 100;

	vector<int>&& bin = dec2bin(n);
	reverse(bin.begin(), bin.end());
	cout << "The number represented in decimal is " << n << endl;
	cout << "The number represented in binary is ";
	for_each(bin.cbegin(), bin.cend(), [=](int n) { std::cout << n; });
	cout << endl;
}

