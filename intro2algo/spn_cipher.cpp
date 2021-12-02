#include <tchar.h>

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <iterator>
#include <bitset>
#include <random>
#include <fstream>

#include "common.h"

using namespace std;

const size_t l = 4;
const size_t m = 4;
const size_t N = 4;

int spn_cipher(ifstream& in, ofstream& out, ofstream& out2)
{
	unsigned char K[N + 1][m]{ { 0b0011, 0b1010, 0b1001, 0b0100},
		{0b1010, 0b1001, 0b0100, 0b1101},
		{0b1001, 0b0100, 0b1101, 0b0110},
		{0b0100, 0b1101, 0b0110, 0b0011},
		{0b1101, 0b0110, 0b0011, 0b1111} };

	random_device seed_gen;
	mt19937 engine(seed_gen());

	vector<int> s_perm(16), inv_s_perm(16);
	iota(s_perm.begin(), s_perm.end(), 0);

	shuffle(s_perm.begin(), s_perm.end(), engine);
	for (auto n = 0; n < inv_s_perm.size(); n++)
	{
		*next(inv_s_perm.begin(), *next(s_perm.begin(), n)) = n;
	}

	vector<int> p_perm(16), inv_p_perm(16);
	iota(p_perm.begin(), p_perm.end(), 0);

	shuffle(p_perm.begin(), p_perm.end(), engine);
	for (auto n = 0; n < inv_p_perm.size(); n++)
	{
		*next(inv_p_perm.begin(), *next(p_perm.begin(), n)) = n;
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
	int nblocks = length / ((l * m) / 8);
	unsigned int u0, u1, u2, u3;
	unsigned int v0, v1, v2, v3, v;

	for (size_t n = 0; n < nblocks; n++)
	{
		unsigned short u = *reinterpret_cast<unsigned short*>(buffer + (n << 1));

		for (int r = 0; r < N; r++)
		{
			u0 = (u >> 0) & 0xf;
			u1 = (u >> 4) & 0xf;
			u2 = (u >> 8) & 0xf;
			u3 = (u >> 12) & 0xf;

			u0 ^= K[r][0];			
			u1 ^= K[r][1];
			u2 ^= K[r][2];
			u3 ^= K[r][3];
			
			v0 = s_perm[u0];
			v1 = s_perm[u1];
			v2 = s_perm[u2];
			v3 = s_perm[u3];
			
			v = (v0 << 0) | (v1 << 4) | (v2 << 8) | (v3 << 12);

			if (r < N - 1)
			{
				bitset<l * m> w = v;
				bitset<l * m> t = v;

				for (int i = 0; i < w.size(); i++)
					t[i] = w[p_perm[i]];

				u = t.to_ulong();
			}
			else
				u = v;
		}

		u0 = (u >> 0) & 0xf;
		u1 = (u >> 4) & 0xf;
		u2 = (u >> 8) & 0xf;
		u3 = (u >> 12) & 0xf;

		u0 ^= K[N][0];
		u1 ^= K[N][1];
		u2 ^= K[N][2];
		u3 ^= K[N][3];
		
		v = (u0 << 0) | (u1 << 4) | (u2 << 8) | (u3 << 12);

		*reinterpret_cast<unsigned short*>(buffer + (n << 1)) = v;

	}

	out.write(buffer, length);
	out.close();

	for (size_t n = 0; n < nblocks; n++)
	{
		unsigned short u = *reinterpret_cast<unsigned short*>(buffer + (n << 1));

		for (int r = 0; r < N; r++)
		{
			u0 = (u >> 0) & 0xf;
			u1 = (u >> 4) & 0xf;
			u2 = (u >> 8) & 0xf;
			u3 = (u >> 12) & 0xf;

			u0 ^= K[N - r][0];
			u1 ^= K[N - r][1];
			u2 ^= K[N - r][2];
			u3 ^= K[N - r][3];

			u = (u0 << 0) | (u1 << 4) | (u2 << 8) | (u3 << 12);

			if (r > 0)
			{
				bitset<l* m> w = u;
				bitset<l* m> t = u;

				for (int i = 0; i < w.size(); i++)
					t[i] = w[inv_p_perm[i]];

				u = t.to_ulong();
			}

			u0 = (u >> 0) & 0xf;
			u1 = (u >> 4) & 0xf;
			u2 = (u >> 8) & 0xf;
			u3 = (u >> 12) & 0xf;

			v0 = inv_s_perm[u0];
			v1 = inv_s_perm[u1];
			v2 = inv_s_perm[u2];
			v3 = inv_s_perm[u3];

			u = (v0 << 0) | (v1 << 4) | (v2 << 8) | (v3 << 12);	
		}

		u0 = (u >> 0) & 0xf;
		u1 = (u >> 4) & 0xf;
		u2 = (u >> 8) & 0xf;
		u3 = (u >> 12) & 0xf;

		u0 ^= K[0][0];
		u1 ^= K[0][1];
		u2 ^= K[0][2];
		u3 ^= K[0][3];

		v = (u0 << 0) | (u1 << 4) | (u2 << 8) | (u3 << 12);

		*reinterpret_cast<unsigned short*>(buffer + (n << 1)) = v;

	}

	out2.write(buffer, length);
	out2.close();

	delete[] buffer;

	return 0;
}

int spn_cipher2(ifstream& in, ofstream& out, ofstream& out2)
{
	unsigned char K[N + 1][m]{ { 0b0011, 0b1010, 0b1001, 0b0100},
		{0b1010, 0b1001, 0b0100, 0b1101},
		{0b1001, 0b0100, 0b1101, 0b0110},
		{0b0100, 0b1101, 0b0110, 0b0011},
		{0b1101, 0b0110, 0b0011, 0b1111} };

	random_device seed_gen;
	mt19937 engine(seed_gen());

	vector<int> s_perm(16), inv_s_perm(16);
	iota(s_perm.begin(), s_perm.end(), 0);

	shuffle(s_perm.begin(), s_perm.end(), engine);
	for (auto n = 0; n < inv_s_perm.size(); n++)
	{
		*next(inv_s_perm.begin(), *next(s_perm.begin(), n)) = n;
	}

	vector<int> p_perm(16), inv_p_perm(16);
	iota(p_perm.begin(), p_perm.end(), 0);

	shuffle(p_perm.begin(), p_perm.end(), engine);
	for (auto n = 0; n < inv_p_perm.size(); n++)
	{
		*next(inv_p_perm.begin(), *next(p_perm.begin(), n)) = n;
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
	int nblocks = length / ((l * m) / 8);

	for (size_t n = 0; n < nblocks; n++)
	{
		unsigned short u = *reinterpret_cast<unsigned short*>(buffer + (n << 1));

		for (int r = 0; r < N; r++)
		{
			u ^= (K[r][0] << 0) | (K[r][1] << 4) | (K[r][2] << 8) | (K[r][3] << 12);
			u = (s_perm[(u & 0x000f) >> 0] << 0) | (s_perm[(u & 0x00f0) >> 4] << 4) | \
				(s_perm[(u & 0x0f00) >> 8] << 8) | (s_perm[(u & 0xf000) >> 12] << 12);

			if (r < N - 1)
			{
				bitset<l* m> w = u;
				bitset<l* m> t = u;

				for (int i = 0; i < w.size(); i++)
					t[i] = w[p_perm[i]];

				u = t.to_ulong();
			}
		}

		u ^= (K[N][0] << 0) | (K[N][1] << 4) | (K[N][2] << 8) | (K[N][3] << 12);
		*reinterpret_cast<unsigned short*>(buffer + (n << 1)) = u;
	}

	out.write(buffer, length);
	out.close();

	for (size_t n = 0; n < nblocks; n++)
	{
		unsigned short u = *reinterpret_cast<unsigned short*>(buffer + (n << 1));

		for (int r = 0; r < N; r++)
		{
			u ^= (K[N - r][0] << 0) | (K[N - r][1] << 4) | (K[N - r][2] << 8) | (K[N - r][3] << 12);

			if (r > 0)
			{
				bitset<l* m> w = u;
				bitset<l* m> t = u;

				for (int i = 0; i < w.size(); i++)
					t[i] = w[inv_p_perm[i]];

				u = t.to_ulong();
			}

			u = (inv_s_perm[(u & 0x000f) >> 0] << 0) | (inv_s_perm[(u & 0x00f0) >> 4] << 4) | \
				(inv_s_perm[(u & 0x0f00) >> 8] << 8) | (inv_s_perm[(u & 0xf000) >> 12] << 12);
		}

		u ^= (K[0][0] << 0) | (K[0][1] << 4) | (K[0][2] << 8) | (K[0][3] << 12);

		*reinterpret_cast<unsigned short*>(buffer + (n << 1)) = u;
	}

	out2.write(buffer, length);
	out2.close();

	delete[] buffer;

	return 0;
}

void spn_cipher_test(int argc, TCHAR* argv[])
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

	spn_cipher2(ifs, ofs, ofs2);

}