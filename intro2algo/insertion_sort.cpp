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

void insertion_sort_test()
{
	vector<int> v{ 12, 4, 7, 6, 15, 2, 17, 9 };

	cout << "before sorting:";
	for_each(v.cbegin(), v.cend(), [](auto n) { cout << n << " ";  });
	cout << '\n';

	insertion_sort(v);

	cout << "after sorting: ";
	for_each(v.cbegin(), v.cend(), [](auto n) { cout << n << " ";  });
	std::cout << '\n';
}
