#include <iostream>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

int cut(int n, map<int, int>& profits, map<int, int>& res)
{
	if (n == 0)
		return n;

	if (res.find(n) != res.end())
		return res[n];

	vector<int> tmp;

	for (int i = 1; i <= n; i++)
		tmp.push_back(profits[i] + cut(n - i, profits, res));

	int r = *max_element(tmp.begin(), tmp.end());
	res[n] = r;

	return r;
}

int cut(int n, map<int, int>& profits, vector<int>& r, vector<int>& s)
{
	r.resize(n + 1);
	s.resize(n + 1);
	
	r[0] = 0;
	s[0] = 0;

	for (int i = 1; i <= n; i++)
	{
		auto q = numeric_limits<int>::min();
		for (int j = 1; j <= i; j++)
		{
			if (q < profits[j] + r[i - j])
			{
				q = profits[j] + r[i - j];
				s[i] = j;

			}
		}
		r[i] = q;
	}

	return *r.rbegin();
}


void cut_test()
{
	int n = 4;
	map<int, int> profits = {
		{1, 1},
		{2, 5},
		{3, 8},
		{4, 9},
		{5, 10},
		{6, 17},
		{7, 17},
		{8, 20},
		{9, 24},
		{10, 30},
	};

	map<int, int> res;
	int r = cut(n, profits, res);

	cout << "profit: " << r << endl;

	res[n] = r;

	for (const auto& [key, value] : res)
	{
		cout << key << " : " << value << endl;
	}

	vector<int> rv, s;

	r = cut(n, profits, rv, s);
	cout << "profit: " << r << endl;

	cout << "list of r:" << endl;
	for (const auto value : rv)
	{
		cout << value << "\t";
	}
	cout << endl;

	cout << "list of s:" << endl;
	for (const auto value : s)
	{
		cout << value << "\t";
	}
	cout << endl;

}

