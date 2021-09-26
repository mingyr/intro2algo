#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void insertion_sort(vector<int>& v)
{
	for (int i = 1; i < v.size(); i++)
	{
		auto key = v[i];
		int j = i - 1;

		for (; j >= 0 && v[j] > key; j --)
		{
			v[j + 1] = v[j];
		}
		v[j + 1] = key;
	}
}

static void print(int n)
{
	std::cout << " " << n;
}

void insertion_sort_test(vector<int>& v)
{
    cout << "before sorting:";
    for_each(v.cbegin(), v.cend(), print);
    cout << '\n';

	insertion_sort(v);

    cout << "after sorting: ";
    for_each(v.cbegin(), v.cend(), print);
    std::cout << '\n';
}
