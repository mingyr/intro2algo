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
}

