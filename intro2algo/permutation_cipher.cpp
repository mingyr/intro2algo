#include <tchar.h>

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <iterator>
#include <random>
#include <fstream>

#include "common.h"

using namespace std;

static const int perm_len = 6;

int permutation_cipher(ifstream& in, ofstream& out, ofstream& out2)
{
	vector<int> perm(perm_len), inv_perm(perm_len);
	iota(perm.begin(), perm.end(), 0);

	random_device seed_gen;
	mt19937 engine(seed_gen());
	shuffle(perm.begin(), perm.end(), engine);

	for (int n = 0; n < inv_perm.size(); n++)
	{
		*next(inv_perm.begin(), *next(perm.begin(), n)) = n;
	}
	
	// get length of file:
	in.seekg(0, in.end);
	int length = in.tellg();
	in.seekg(0, in.beg);

	char* buffer = new char[length];

	std::cout << "Reading " << length << " characters... ";
	// read data as a block:
	in.read(buffer, length);

	in.close();

	// ...buffer contains the entire file...
	int nblocks = length / perm_len;
	for (int n = 0; n < nblocks; n++)
	{
		for (int j = 0; j < perm_len; j++)
		{
			buffer[n * perm_len + j] = buffer[n * perm_len + perm[j]];
		}
	}

	out.write(buffer, length);
	out.close();

	for (int n = 0; n < nblocks; n++)
	{
		for (int j = 0; j < perm_len; j++)
		{
			buffer[n * perm_len + j] = buffer[n * perm_len + inv_perm[j]];
		}
	}

	out2.write(buffer, length);
	out2.close();

	delete[] buffer;

	return 0;

}

void permutation_cipher_test(int argc, TCHAR* argv[])
{
	string infile = utf8_encode(argv[1]);
	string outfile = utf8_encode(argv[2]);
	string outfile2 = utf8_encode(argv[3]);

	ifstream ifs{ infile, ios::binary | ios::in };
	if (!ifs)
	{
		cout << "couldn't open " << infile << " for reading" << endl;
		return;
	}

	ofstream ofs{ outfile, ios::binary | ios::out | ios::trunc };
	if (!ofs)
	{
		cout << "couldn't open " << outfile << " for writing" << endl;
		return;
	}

	ofstream ofs2{ outfile2, ios::binary | ios::out | ios::trunc };
	if (!ofs2)
	{
		cout << "couldn't open " << outfile2 << " for writing" << endl;
		return;
	}

	permutation_cipher(ifs, ofs, ofs2);

}