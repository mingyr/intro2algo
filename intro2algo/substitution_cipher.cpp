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


int substitution_cipher(ifstream& in, ofstream& out, ofstream& out2)
{
	vector<int> perm(26), inv_perm(26);
	iota(perm.begin(), perm.end(), 0);

	random_device seed_gen;
	mt19937 engine(seed_gen());
	shuffle(perm.begin(), perm.end(), engine);

	for (auto n = 0; n < inv_perm.size(); n ++)
	{
		*next(inv_perm.begin(), *next(perm.begin(), n)) = n;
	}
	char c;
	while ((c = in.get()) != EOF)
	{
		// encryption
		char e = c;
		if (isupper(c))
		{
			e = tolower('A' + perm[c - 'A']);
			out << e;
		}
		else if (islower(c))
		{
			e = toupper('a' + perm[c - 'a']);
			out << e;
		}
		else
			out << e;

		char d = e;
		// decryption
		if (isupper(e))
		{
			d = tolower('A' + inv_perm[e - 'A']);
			out2 << d;
		}
		else if (islower(e))
		{
			d = toupper('a' + inv_perm[e - 'a']);
			out2 << d;
		}
		else
			out2 << d;

	}

	return 0;
}

void substitution_cipher_test(int argc, TCHAR* argv[])
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
	
	substitution_cipher(ifs, ofs, ofs2);

}