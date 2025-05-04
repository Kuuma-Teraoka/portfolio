
/*
　ラップしているが、std::unordered_setはclassなので継承にしたい。operater<<で、なぜか
{{2, 1, 3}, {6, 5, 4}{3, 2, 1}}
　と出力される。{3,2,1}がbegin()と一致しているから", "がないのはいいとして、for(i : std::unordered_set)で重複が起きるのが意味不明。
　baモデルはvectorを使う。そしてsetはハッシュテーブルから自作する。
*/


#include <unordered_set>
#include <iostream>

template <typename T>
class set
{
public:
    std::unordered_set<T> data ;

    set() = default ;
    set(std::initializer_list<T> init) : data(init) {}

    auto begin() const {return data.begin() ;}
    auto end() const {return data.end() ;}
    auto insert(const T &value) {return data.insert(value) ;}
    bool operator==(const set<T> &other) const {return data == other.data ;}
    bool operator!=(const set<T> &other) const {return data != other.data ;}

    friend std::ostream& operator<<(std::ostream& os, const set& self)
    {
        os << "{" ;
        for(auto &i : self.data)
        {
            if(i == *(self.data.begin()))
                os << i ;
            else
                os << ", " << i ;
        }
        os << "}" ;
        return os ;
    }
} ;

namespace std
{
    template <typename T>
    struct hash<set<T>>
    {
        std::size_t operator()(const set<T>& s) const
        {
            std::size_t result = 0 ;
            for (const auto& x : s) 
                result ^= std::hash<T>{}(x) + 0x9e3779b9 + (result << 6) + (result >> 2) ;
            return result ;
        }
    } ;
}


/*
#include <unordered_set>

template <typename T>
class set : public std::unordered_set<T>
{
public:
    using std::unordered_set<T>::unordered_set ;
} ;
*/

