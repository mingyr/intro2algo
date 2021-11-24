#include <iostream>
#include <vector>

using namespace std; 

int cd_max(vector<int>& v, size_t p, size_t q)
{
    int l = p, r = q;

    int m = (p + q) >> 1;
    if (m < q)
    {
        l = cd_max(v, p, m);
        r = cd_max(v, m + 1, q);
    }

    return v[l] > v[r] ? l : r;
}

void cd_max_test()
{
    vector<int> v{ 1, 13, 5, 271, 9, 2, 99 };
    int m = cd_max(v, 0, v.size() - 1);

    cout << "maximum element: " << v[m] << endl;

}
