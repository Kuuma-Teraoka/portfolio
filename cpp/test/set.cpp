
#include <iostream>
#include "set.hpp"

int main() {
    // `set<set<int>>` を作成
    set<set<int>> sets = {
        { {1, 2, 3} },
        { {3, 1, 2} }, // 同じ集合なので追加されない
        { {4, 5, 6} }  // 異なる集合なので追加される
    };

    std::cout << sets << "\n" ;

    return 0;
}


