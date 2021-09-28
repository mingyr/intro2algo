#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

bool merge(vector<int>& A, size_t p, size_t q, size_t r)
{
	cout << " In " << __func__ << endl;
	cout << "p = " << p << "\t" << "q = " << q << "\t" << "r = " << r << endl;

	size_t n1 = q - p;
	size_t n2 = r - q;

	vector<int> L, R;
	L.resize(n1 + 1);
	R.resize(n2 + 1);

	copy(next(A.begin(), p), next(A.begin(), q), L.begin());
	copy(next(A.begin(), q), next(A.begin(), r), R.begin());

	cout << "L = ";
	for_each(next(A.begin(), p), next(A.begin(), q), [](int n) { cout << n << "\t"; });
	cout << endl;

	cout << "R = ";
	for_each(next(A.begin(), q), next(A.begin(), r), [](int n) { cout << n << "\t"; });
	cout << endl;

	*L.rbegin() = INT_MAX;
	*R.rbegin() = INT_MAX;

	auto l_it = L.begin();
	auto r_it = R.begin();

	for_each(next(A.begin(), p), next(A.begin(), r), [&](auto& n)
		{
			if (*l_it <= *r_it)
			{ 
				n = *l_it;
				l_it ++; 
			}
			else
			{
				n = *r_it;
				r_it ++;	
			}	
		});

	return true;
}

bool merge_sort(vector<int>& A, size_t p, size_t r)
{
	cout << " In " << __func__ << endl;
	cout << "p = " << p << "\t";

	if (r - p > 1)
	{
		auto q = (p + r) >> 1;

		cout << "q = " << q << endl;

		merge_sort(A, p, q);
		merge_sort(A, q, r);
		merge(A, p, q, r);
	}
	
	cout << endl;

	return true;
}

void merge_sort_test()
{
	vector<int> v{ 1, 3, 5, 4, 12, 2, 6, 9 };

	cout << "before sorting:";
	for_each(v.cbegin(), v.cend(), [](auto n) { cout << n << " ";  });
	cout << '\n';

	merge_sort(v, 0, v.size());

	cout << "after sorting: ";
	for_each(v.cbegin(), v.cend(), [](auto n) { cout << n << " ";  });
	cout << '\n';
}
