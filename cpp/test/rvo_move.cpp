#include <iostream>

class matrix {
public:
    matrix()
        {std::cout << "Default constructed: " << (void *)this << "\n" ;}
    matrix(const matrix& other)
        {std::cout<<"Copy constructed: "<<(void *)this<<"<-"<<(void *)&other<<"\n" ;}
    matrix(matrix&& other) noexcept
        {std::cout<<"Move constructed: "<<(void *)this<<"<-"<<(void *)&other<<"\n" ;}
    ~matrix()
        {std::cout << "Destructed: " << (void *)this << "\n" ;}
    matrix& operator=(const matrix& other)
    {
        std::cout << "Copy assigned\n" ;
        return *this ;
    }
    matrix& operator=(matrix&& other) noexcept
    {
        std::cout << "Move assigned: "<<(void *)this<<"<-"<<(void *)&other<<"\n" ;
        return *this ;
    }
};

matrix method(matrix input)
{
    matrix result = input ; // resultがスタック上に生成
    return result ;         // resultが戻り値としてムーブされる
}

int main()
{
    matrix mat_a ;
    matrix mat_b = method(mat_a) ; // ムーブコンストラクタ
    mat_b = method(mat_a) ;        // ムーブ代入演算子
    return 0 ;
}

