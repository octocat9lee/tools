#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

template <typename T>
void printVec(const vector<T> vec)
{
    for(const auto& e : vec)
    {
        cout << e << "\t";
    }
    cout << endl;
}

int main()
{
    vector<int> vec = {9, 3, 7, 1, 5};
    printVec(vec);

    make_heap(vec.begin(), vec.end());
    printVec(vec);

    vec.push_back(99);
    push_heap(vec.begin(), vec.end());
    printVec(vec);

    pop_heap(vec.begin(), vec.end());
    printVec(vec);
    vec.pop_back();

    sort_heap(vec.begin(), vec.end());
    printVec(vec);
    return 0;
}
