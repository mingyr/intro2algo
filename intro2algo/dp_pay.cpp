#include <iostream>
#include <limits>
#include <map>
#include <algorithm>

using namespace std;

int pay(int n, map<int, int>& res)
{
	int cost = numeric_limits<int>::max();

	if (res.find(n) != res.end())
		return res[n];

	if (n == 0)
		return 0;
	if (n - 11 >= 0)
	{
		if (res.find(n - 11) == res.end())
		{
			auto tmp = pay(n - 11, res);
			res[n - 11] = tmp;
		}
		cost = min<int>(cost,  res[n - 11] + 1);
	}
	if (n - 5 >= 0)
	{
		if (res.find(n - 5) == res.end())
		{
			auto tmp = pay(n - 5, res);
			res[n - 5] = tmp;
		}
		cost = min<int>(cost, res[n - 5] + 1);
	}
	if (n - 1 >= 0)
	{
		if (res.find(n - 1) == res.end())
		{
			auto tmp = pay(n - 1, res);
			res[n - 1] = tmp;
		}
		cost = min<int>(cost, res[n - 1] + 1);
	}

	return cost;
}

void pay_test()
{
	int n = 15;
	map<int, int> res;
	int cost = pay(n, res);

	cout << "cost: " << cost << endl;

	res[n] = cost;

	for (const auto& [key, value] : res) 
	{
		cout << key << " : " << value << endl;
	}
}

