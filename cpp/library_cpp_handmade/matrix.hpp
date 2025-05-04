
#ifndef _matrix_hpp
#define _matrix_hpp

/*
　numpyを参考にしてcppで作った自作行列演算ライブラリ。numpy以外にもcppの行列演算ライブラリなども全て使いこなしたうえで、それらよりもいいと思えるライブラリを思いついて、それをつくれたら理想ではあるのだが、numpyをちょっとかじった程度である。
　いろんなライブラリを見てしまうと、なにか先入観のようなでてしまう気もしていて、素人がセンスのままになにも参考にせず行列演算ライブラリを作るというのもなにか新しいものができそうな気もする。
　もしそれで同じものができたのなら、少し遠回りではあるがより深い理解に得られるだろうし、とりあえず作ってみようと思う。

　列優先にしている。列ベクトルのリストを行列としてつかうときに、行優先の行列から列ベクトルを抜くとメモリアクセスがとびとびになってしまう。numpyは行優先で、
3, 4, 2, 5, 2, 1, 8, 2, 5, 7, 9, 6
　というメモリを(2, 3, 2)というshapeで解釈すると、
[
[3 4]
[2 5]
[2 1]
]
[
[8 2]
[5 7]
[9 6]
]
　という3次元行列になるが、これを列優先で解釈すると、
3 2 2 | 8 5 9
4 5 1 | 2 7 6
　と解釈する。この書き方ちょっといまいちか。


　基本的にほとんどのインスタンス関数は1つ以上の行列を入力すると、行列がひとつ出力される関数である。そして、うちひとつをselfとして引数を省略できるのがインスタンス関数の良いところである。
　計算した行列を、引数distで受け取ったメモリに書き込む書き込み型、新しくメモリを確保して返す生成型、selfを書き換える自己書き換え型の3パターンがある。

　行列積であれば結果は形が違うので自己書き換え型は不可能で、書き込み型と生成型になる。a.mul(b)で生成型, a.mul(b, c)で書き込み型にする。a * bでa.mul(b)を呼び出すようにして、ライブラリに切り替えたいならa.lapacke_mul(b)を呼ぶようにする。
　fillは自己書き換え型。fillは固定値であり入力を必要としないので自己書き換え型か生成型しか考えられない。生成型はコンストラクタに組み込んでいる。自己書き換え型もコンストラクタがあるのでなかなか使うことはないのだが、スライスで一部分だけ定数で埋めるというのはある。
　double->double関数を渡してすべての要素を変換する関数。3パターンのうち、3つ目の自己書き換えをfillで実装する。mat.fill(calc::tanh)ですべての要素をtanhで変換した値に書き換える。

　constはインスタンス変数の書き換えに関するもので、ポインタであればポインタ自体の書き換えが不可になるだけで、それが指すメモリの中身は書き換え可能である。しかし、それだと直感的ではないので、行列の中身を書き換えるのはconstなし、書き換えないのはconstありにしている。


　operator=。cpp.txtのgetitem参照。numpyのmat_a(slice(), 3) = mat_bとmat_a = mat_cの両方を再現したい。前者は左辺がビュー、後者は左辺がビューでないで場合分け。前者はrvoオフにすると左辺がムーブコンストラクタによってmain()に確保されるときにコピーでビューではなくなってしまう。コンストラクタで右辺が一時オブジェクトのとき、つまりmatrix(matrix &&)でビューでもなんでもムーブするようにするか。
　右辺が右辺値か左辺値か、そしてこの二つの分岐それぞれに、右辺左辺両方でビューかビューでないかの4パターンがある。
　まず右辺が左辺値のとき。左辺がビューならshape一致前提で右辺ビューかどうか関係なくコピー、左辺がビューでないならshape不一致で同じshapeでリソース再確保してコピー、shape一致でコピー、このとき右辺がビューでないならdata+iでアクセスして高速化。リソース再確保は、(*this).~matrix()でデストラクタを呼べるが、shapeやstridesはどうせ同じ大きさなのでdataだけ再確保。
　echo_state_networkのような、matrixをインスタンス変数に持つもので、matrix()ではdata=NULL状態で、あとからw_x = matrix(...とする場合でも、free(NULL)はエラーをはかないので問題なし。なのでshapeやstridesは再確保しないというのはmatrix()でshapeやstridesを確保していることが前提。
　右辺が右辺値なら、ビューだとしてもムーブするようにする。ただし、左辺がビューであるときはコピーにする。左辺がビューであるときはただ元となるインスタンスの一部を指定して書き換えたいときなので。


　戻り値はconst matrix &でも、matrix &でもvoidでもどれでもいいはず。a = b = c、つまり、a = (b = c)ならconstでいいし、(a = b).fill(1)ならmatrix &にする必要がある。

　matrix(matrix &&)とmatrix(const matrix &)。右辺が右辺値か左辺値かで、operator=と同様に場合分けして考える。operator=との違いは、operator=は既存のインスタンスへの代入処理だが、コンストラクタはインスタンスを新しく生成するときの処理であること。なので、operator=より単純。
　右辺が左辺値のときつまりmatrix(const matrix &)はコピー。
　右辺が右辺値のときつまりmatrix(matrix &&)はムーブ。右辺がビューである場合もムーブにする。
*/

#include <iostream> // std::cout
#include <cstdlib> // malloc()
#include <lapacke.h> // LAPACKE_dgeev(), LAPACKE_dgetrf(), LAPACKE_dgetri(), 
#include <cblas.h> // cblas_dgemm()
#include <vector> // std::vector<> to_vector()

#include "file.hpp"
#include "slice.hpp"
#include "progress.hpp"
#include "str.hpp"
#include "calc.hpp"
#include "bin.hpp"

class matrix
{
public: 
    double *data ;
    int *shape ;
    int *strides ;
    int is_view ; // mat(slice())やmat.T()で切り出した行列を、dataの所有権を持たない一時的な行列、ビューとする。ビューのdataはデストラクタで解放しない。このようなmatrixはis_viewというフラグを1にする。逆に、ビュー以外の行列はstridesは(1, shape[0])で固定である。stridesで判断してもいいのだが、mat(slice(), slice())や、mat.T().T()としたときにstridesは(1, shape[0])となってしまうので解放されてしまう。最も、このようなビューをつくることはないのだが。

    matrix(void) : is_view(0), data(NULL)
    {
        //std::cout << "matrix():" << (void *)this << std::endl ;
        shape = bin::malloc<int>(2) ;
        strides = bin::malloc<int>(2) ;
    }
    matrix(int rows, int cols) : matrix()
    {
        shape[0] = rows ;
        shape[1] = cols ;
        strides[0] = 1 ;
        strides[1] = shape[0] ;
        data = bin::malloc<double>(shape[0] * shape[1]) ;
    }
    matrix(int rows, int cols, double value) : matrix(rows, cols)
        {(*this).fill(value) ;}
    // *.cmatだったときは、とりあえず列優先で読み込んで、T()で転置したビューをつくり、=でコピーする。このとき、(*this) = (*this).T()とすると、copy()で(0,0),(0,1)とコピーしているときに、(*this).T().dataも書き変わっていくので上手くいかない。tmpにコピー初期化で一時的なメモリにコピーして、そのあとに読み込む。
    // matrix tmp = (*this).T()だとRVOがオンだと(*this).T()でT()のスタックフレームで組み立てられるはずのmatrixが直接tmpに作られるため、ムーブ初期化のビューはコピー初期化にする挙動を無視してtmpにT()をそのまま組み立ててしまう。matrix tmp ; tmp =とすれば、RVOがはたらいても、T()のmatrixが構築されるのはoperator=(const matrix &)であり、この場合おそらくmainの一時オブジェクト領域に構築されるので問題なく動作する。詳しくはcpp.txt。といってもまだ理解には至っていない。
    // esn用にファイルストリームのラッパークラスであるfileにも対応したい。fileにstr file_nameのインスタンス変数を足したので、csr(file)と同様、matrix(file)だけでいい。
    // matrix(matrix &&)やoperator=(matrix &&)でビューもムーブして左辺値にするようにした。なので、左辺値であるT()をムーブしてtmp1に右辺値として保存してから、tmp2にコピーして、(*this) = tmp2でoperator=(const matrix &)でshape不一致で再確保してからコピーにした。
    matrix(const file &input_file) : matrix()
    {
        input_file.read(shape, sizeof(int) * 2) ;
        strides[0] = 1 ;
        strides[1] = shape[0] ;
        data = bin::malloc<double>(shape[0] * shape[1]) ; 
        if(input_file.file_name.endswith(".cmat"))
            for(int i = 0 ; i < shape[0] ; i++)
                for(int j = 0 ; j < shape[1] ; j++)
                    input_file.read(&(*this)(i, j), sizeof(double)) ;
        else
            input_file.read(data, sizeof(double) * shape[0] * shape[1]) ;
    } 

    // double a[9] = {1,2,...,9} ; matrix mat(a) ;用。静的確保したaをそのままdataにいれるとfree()とか関数終了時の解放とかで問題が起きそうなのでコピーする。
    matrix(double *input, int rows, int cols) : matrix()
    {
        shape[0] = rows ;
        shape[1] = cols ; 
        strides[0] = 1 ;
        strides[1] = shape[0] ;
        data = (double *)malloc(sizeof(double) * shape[0] * shape[1]) ;
        for(int i = 0 ; i < rows * cols ; i++)
            data[i] = input[i] ;
    }
    // コピー初期化。operator=を使う。shape一致で再確保は行われずコピーされる。
    matrix(const matrix& other) : matrix(other.shape[0], other.shape[1])
    {
        //std::cout << "matrix(const matrix &):" << (void *)this ; 
        (*this) = other ;
        //std::cout << " has " << (void *)data << "\n" ;
    }
    matrix(matrix &&other) noexcept
    {
        //std::cout << "matrix(move):" << (void *)this ;
        shape = other.shape ;
        strides = other.strides ;
        data = other.data ;
        is_view = other.is_view ;
        other.shape = NULL ;
        other.strides = NULL ;
        other.data = NULL ;

        //std::cout << " has " << (void *)data << "\n" ;
    }
    
    // ビューのときはdataを解放しない。
    ~matrix()
    {
        //std::cout << "~matrix():" << (void *)this << std::endl ;
        if(is_view == 0)
            free(data) ;
        free(shape) ;
        free(strides) ;
    }


    /*
    x = x(x.shape[1]-1) ;
    　とすると、左辺のfree(x.data)で右辺が不定値になってしまうので、 
    matrix tmp1 = x(x.shape[1]-1) ; // move
    matrix tmp2 = tmp1 ; // copy
    x = tmp2 ;
    　としなければいけないのが冗長なので、
    x = x(x.shape[1]-1).copy()
    　で一時オブジェクトのコピーを作りxを捨ててxに入れられるにしたい。ようは、copy() matrix &&thisで、新しくthisと同じshapeでmatrixを確保してコピーして返す関数。copy(const matrix &other)はshape一致前提でotherをselfにコピーする関数で、方向が違うので注意。
    　ここで、x(299)は右辺値だが、x(299).copy()としたときに、copy()に渡されるthisはmatrix &&thisなのか、matrix &thisなのかでmatrix result = (*this)の挙動が変わる。どうやら、
    matrix &this = x(299)
    　というようにして引数であるthisがあたえれるわけだが、これにより、かならず左辺値になるらしい。ようは、たとえ参照型の参照先が右辺値であろうと、参照型は左辺値として扱われるのと同様。
    　ただ、ここらへんはまだ堀がいがありそうではある。そもそもC言語でmethod(method(3), 3)としたときに、method(3)のスタック領域にあるresultはmain()に保存されてからmethod()の引数に保存されるのか、それとも直接保存されるのかという問題もある。これはgptにきこう。
    　とりあえず必ずコピーが行われるような書き方でcopy()を作る。
    
    */
    matrix copy()
    {
        matrix result(shape[0], shape[1]) ;
        result.copy((*this)) ;
        return result ;

        //matrix result = (*this) ;
        //return result ;

        //return matrix((*this)) ;

        //return (*this) ;
        
        //return std::move((*this)) ; //これだけmoveが呼ばれる。copy() &&としてもstd::moveがないとコピー初期化になる。
    }

    // shapeが同じであることを前提にdataをotherからselfにコピーする関数。ビューでも可能。ビューでないときは高速化のためdataアクセスにしている。あと、列優先なので列ごとにアクセスしている。
    matrix &copy(const matrix &other)
    {
        //std::cout << "matrix::copy(): " << (void *)this << " <- " << (void *)&other << "\n" ;
        if(shape[0] != other.shape[0] || shape[1] != other.shape[1])
            throw ("matrix::operator=(): shape[0] != other.shape[0] || shape[1] != other.shape[1]") ;
        if(is_view || other.is_view) 
            for(int j = 0 ; j < shape[1] ; j++)
                for(int i = 0 ; i < shape[0] ; i++) 
                    (*this)(i, j) = other(i, j) ;
        else
            for(int i = 0 ; i < shape[0] * shape[1] ; i++) 
                data[i] = other.data[i] ;
        return (*this) ;
    }
    matrix &operator=(const matrix &other)
    {
        //std::cout << "matrix::operator=(const matrix &): " << (void *)this << " <- " << (void *)&other << "\n" ;
        if(is_view)
            return (*this).copy(other) ; 
        else if(data != NULL && shape[0] == other.shape[0] && shape[1] == other.shape[1])
            return (*this).copy(other) ; 
        else
        { 
            shape[0] = other.shape[0] ;
            shape[1] = other.shape[1] ;
            strides[0] = 1 ;
            strides[1] = shape[0] ; 
            free(data) ;
            data = (double *)malloc(sizeof(double) * shape[0] * shape[1]) ; 
            return (*this).copy(other) ;
        }   
    }
    matrix &operator=(matrix &&other) noexcept
    {
        //std::cout << "matrix::operator=(matrix &&): " << (void *)this << " <- " << (void *)&other << "\n" ;
        if(is_view)
            return (*this).copy(other) ;
        else
        {
            (*this).~matrix() ;
            shape = other.shape ;
            strides = other.strides ;
            data = other.data ;
            is_view = other.is_view ;
            other.shape = NULL ;
            other.strides = NULL ;
            other.data = NULL ;
            return (*this) ;
        }
    }



    // constありなしで冗長だが、constなしからconstありは呼び出しはできるが返ってくるビューがconstになってしまい、constありからconstなしはそもそも呼び出しができない。
    // 書き換えをしないのであればビューである必要はない。しかし、もうビューを実装しているので、書き換えありのコードにconstをつけるだけなのでビューをつかう。
    // operater()がエラーハンドリングありで、getがエラーハンドリングなし。
    // mat({1}, {}), mat(1, {}), mat({}, 1)としたときに、matrix(int, int)が呼ばれてしまう。つまり、intは1要素のinitializer listを受け取ってしまう。なので、matrix(slice,slice)を呼ぶようにする。 

    double &get(int i, int j)
        {return *(data + i * strides[0] + j * strides[1]) ;}

    double &operator()(int i, int j)
    { 
        if(i < 0 || shape[0] <= i)
            throw std::out_of_range
                (std::format("matrix::operator(): row;{} out of [0,{}]", i, shape[0])) ; 
        if(j < 0 || shape[1] <= j)
            throw std::out_of_range
                (std::format("matrix::operator(): col:{} out of [0,{}]", j, shape[1])) ;
        return (*this).get(i, j) ;
    }
    matrix operator()(std::initializer_list<int> i, std::initializer_list<int> j)
        {return (*this)(slice(i), slice(j)) ;}
    matrix operator()(int i, std::initializer_list<int> j)
        {return (*this)(slice(i), slice(j)) ;}
    matrix operator()(std::initializer_list<int> i, int j)
        {return (*this)(slice(i), slice(j)) ;}
    matrix operator()(slice i, slice j)
    {
        i.setend(shape[0]) ;
        if(shape[0] < i.start || i.end < 0 || shape[0] < i.end)
            throw std::out_of_range("matrix::operator()(slice, slice): shape[0] < i.start || i.end < 0 || shape[0] < i.end") ;

        j.setend(shape[1]) ;
        if(shape[1] < j.start || j.end < 0 || shape[1] < j.end)
            throw std::out_of_range("matrix::operator()(slice, slice): shape[1] < j.start || j.end < 0 || shape[1] < j.end") ;
        
        matrix result ;
        result.shape[0] = i.time() ;
        result.shape[1] = j.time() ;
        // a(3)(slice(0,3), slice())としたときに、strides=(1, shape[0])前提だと上手くいかないので。ただ、これだとほかで問題が起きた気がする。
        result.strides[0] = strides[0] * i.step ;// i.step ;
        result.strides[1] = strides[1] * j.step ;//shape[0] * j.step ;
        result.data = (double *)(&((*this)(i.start, j.start))) ; 
        result.is_view = 1 ;
        return result ;
    } 
    matrix operator()(slice i)
        {return (*this)(slice(), i) ;} 


    double get(int i, int j) const
        {return *(data + i * strides[0] + j * strides[1]) ;}
    double operator()(int i, int j) const
    { 
        if(i < 0 || shape[0] <= i)
            throw std::out_of_range
                (std::format("matrix::operator(): row;{} out of [0,{}]", i, shape[0])) ; 
        if(j < 0 || shape[1] <= j)
            throw std::out_of_range
                (std::format("matrix::operator(): col:{} out of [0,{}]", j, shape[1])) ;
        return (*this).get(i, j) ;
    }
    matrix operator()(std::initializer_list<int> i, std::initializer_list<int> j) const
        {return (*this)(slice(i), slice(j)) ;} 
    matrix operator()(int i, std::initializer_list<int> j) const
        {return (*this)(slice(i), slice(j)) ;}
    matrix operator()(std::initializer_list<int> i, int j) const
        {return (*this)(slice(i), slice(j)) ;}
    const matrix operator()(slice i, slice j) const
    {
        i.setend(shape[0]) ;
        if(shape[0] < i.start || i.end < 0 || shape[0] < i.end)
            throw std::out_of_range("matrix::operator()(slice, slice): shape[0] < i.start || i.end < 0 || shape[0] < i.end") ;

        j.setend(shape[1]) ;
        if(shape[1] < j.start || j.end < 0 || shape[1] < j.end)
            throw std::out_of_range("matrix::operator()(slice, slice): shape[1] < j.start || j.end < 0 || shape[1] < j.end") ;
        
        matrix result ;
        result.shape[0] = i.time() ;
        result.shape[1] = j.time() ;
        result.strides[0] = strides[0] * i.step ;// i.step ;
        result.strides[1] = strides[1] * j.step ;//shape[0] * j.step ;
        result.data = data + i.start * strides[0] + j.start * strides[1] ;
        result.is_view = 1 ;
        return result ; 
    }
    const matrix operator()(slice i) const 
        {return (*this)(slice(), i) ;} 

    // 同じメモリを参照した所有権をもたない一時的なmatrixを生成して返す関数 
    matrix T()
    {     
        matrix result ;
        result.shape[0] = shape[1] ;
        result.shape[1] = shape[0] ;
        result.strides[0] = strides[1] ;
        result.strides[1] = strides[0] ;
        result.data = data ; 
        result.is_view = 1 ; 
        return result ;
    }
    const matrix T() const
    {     
        matrix result ;
        result.shape[0] = shape[1] ;
        result.shape[1] = shape[0] ;
        result.strides[0] = strides[1] ;
        result.strides[1] = strides[0] ;
        result.data = data ; 
        result.is_view = 1 ;
        return result ;
    }
    
 

    // 列優先にしたので(0,0), (1,0), ...と書き込む。 
    const matrix &write(const file &output_file) const
    { 
        output_file.write(shape, sizeof(int) * 2) ;
        if(output_file.file_name.endswith(".cmat"))
            for(int i = 0 ; i < shape[0] ; i++)
                for(int j = 0 ; j < shape[1] ; j++)
                {
                    double tmp = ((*this)(i, j)) ;
                    output_file.write(&tmp, sizeof(double)) ;
                }
        else if(is_view)
        {
            for(int j = 0 ; j < shape[1] ; j++)
                for(int i = 0 ; i < shape[0] ; i++)
                {
                    double tmp = ((*this)(i, j)) ;
                    output_file.write(&tmp, sizeof(double)) ;
                }
        }
        else
            output_file.write(data, sizeof(double) * shape[0] * shape[1]) ; 
        return (*this) ;
    }
    const matrix &write(const str &file_name) const
    {
        (*this).write(file(file_name, "x")) ;
        std::cout << "generated: " << file_name << "\n" ;
        return (*this) ;
    }
    const matrix &write(const char *file_name) const
        {return (*this).write(str(file_name)) ;}


    friend std::ostream& operator<<(std::ostream& os, const matrix& mat)
    {
        for(int i = 0 ; i < mat.shape[0] ; i++)
        {
            for(int j = 0 ; j < mat.shape[1]; j++)
                os << mat(i, j) << " " ;
            os << "\n" ;
        }
        return os ;
    }


    

    // for(matrix i : mat) i = 3 ;で、mat(i) = 3とするためのイテラブル化。endが返すのはただ列数の範囲が0~29ならcol_indexが30であるIteratorを生成して返せばいい。ただ、mat(30)としたら範囲外アクセスでエラーになってしまう。しかし、iをインクリメントてしていき、i.col_indexが、end()の30と同じになって、!=が0を返したときにforループは終了し、*は呼ばれずmat(30)が呼ばれることもないので問題ない。cpp.txtにfor( : )の内部処理を書いているので参照。 
    class Iterator
    {
    public:
        matrix &parent ;
        int col_index ;
        // begin()とend()を返すときに呼ぶ。
        Iterator(matrix &parent, int index): parent(parent), col_index(index) {}
        matrix operator*() {return parent(col_index) ;}
        void operator++() {++col_index ;}
        bool operator!=(const Iterator &other) const
            {return col_index != other.col_index ;}
    } ;
    Iterator begin() {return Iterator((*this), 0) ;}
    Iterator end() {return Iterator((*this), shape[1]) ;}

    



    matrix &add(const matrix &other, matrix &dist) const
    {
        for(int i = 0 ; i < dist.shape[0] ; i++)
            for(int j = 0 ; j < dist.shape[1] ; j++)
                dist(i, j) = (*this)(i, j) + other(i, j) ;
        return dist ;
    }
    matrix add(const matrix &other) const
    {
        matrix result = (*this) ;
        return (*this).add(other, result) ;
    }
    friend matrix operator+(const matrix &arg1, const matrix &arg2)
        {return arg1.add(arg2) ;}
    // addの自己書き換え型。(*this)(i, j) = (*this)(i, j) + other(i, j)は右辺計算語に代入なので問題ない。
    matrix &operator+=(const matrix &other)
        {return (*this).add(other, (*this)) ;}


    matrix &add(const double other, matrix &dist) const
    { 
        for(int i = 0 ; i < dist.shape[0] ; i++) 
            for(int j = 0 ; j < dist.shape[1] ; j++)
                dist(i, j) = (*this)(i, j) + other ;
        return dist ;
    }
    matrix add(const double other) const
    {
        matrix result = (*this) ;
        return (*this).add(other, result) ;
    } 
    friend matrix operator+(const matrix &arg1, const double arg2)
        {return arg1.add(arg2) ;}
    friend matrix operator+(const double arg1, const matrix &arg2)
        {return arg2 + arg1 ;}
    matrix &operator+=(const double other)
        {return (*this).add(other, (*this)) ;}






    matrix &sub(const matrix &other, matrix &dist) const
    {
        dist = (*this) + -1 * other ;
        return dist ; 
    }
    matrix sub(const matrix &other) const
    {
        matrix result = (*this) ;
        return (*this).sub(other, result) ;
    }
    friend matrix operator-(const matrix &arg1, const matrix &arg2)
        {return arg1.sub(arg2) ;} 
    matrix &operator-=(const matrix &other)
        {return (*this).sub(other, (*this)) ;}


    matrix &sub(const double other, matrix &dist) const
    {
        dist = (*this) + -1 * other ;
        return dist ;
    }
    matrix sub(const double other) const
    {
        matrix result = (*this) ;
        return (*this).sub(other, result) ;
    }
    friend matrix operator-(const matrix &arg1, const double arg2)
        {return arg1.sub(arg2) ;} 
    friend matrix operator-(const double arg1, const matrix &arg2)
        {return arg2 - arg1 ;}
    matrix &operator-=(const double other)
        {return (*this).sub(other, (*this)) ;}



    // shape一致前提だが、infer_linksではn次元列ベクトルとn*n行列でn*n行列をつくる箇所があり、numpyでは列ベクトルを横に複製してn*n行列にしてからn*n行列をアダマール積をとっている。ようは、列ベクトルと行列の組み合わせにも対応している。なのでこれもそうする。
    matrix &hadamard(const matrix &other, matrix & dist) const 
    {
        if(shape[1] == 1)
            for(int i = 0 ; i < dist.shape[0] ; i++)
                for(int j = 0 ; j < dist.shape[1] ; j++)
                    dist(i, j) = (*this)(i, 0) * other(i, j) ;
        else
            for(int i = 0 ; i < dist.shape[0] ; i++)
                for(int j = 0 ; j < dist.shape[1] ; j++)
                    dist(i, j) = (*this)(i, j) * other(i, j) ;
        return dist ;
    }
    // 列ベクトルと行列に対応するため、otherのshapeに。
    matrix hadamard(const matrix &other) const
    {
        matrix result = other ;
        return (*this).hadamard(other, result) ;
    }

    // distはCBLAS_TRANSPOSE引数がないので、転置は許されない。まあdistを指定するのはa.mul(b, c)というようにつかうときだけで、せいぜいesn.input()くらいであり、cはただforループでメモリ確保をしないようにする目的なので、distでビューを指定するのはなかなかない。
    matrix &mul(const matrix &other, matrix &dist) const
    {
        if(shape[1] != other.shape[0])
            throw std::invalid_argument(std::format("matrix::mul(other, dist): shape[1]({}) != other.shape[1]({})", shape[1], other.shape[0])) ;
        if(is_T() == -1 || other.is_T() == -1 || dist.is_T() != 0)
        {
            std::cout << "." ;
            return mul_1(other, dist) ;
        }
        else
            return mul_blas(other, dist) ;

        /*if(shape[1] % 100 == 0 && other.is_view == 0)
            return mul_100(other, dist) ;
        else if(shape[1] % 10 == 0)
            return mul_10(other, dist) ;
        else
            return mul_1(other, dist) ;*/
    }
    // stridesからmul_blas()に使えるかどうか判断する関数。列優先なら行方向のstrides[0]が1なら使える。転置なら行優先としてとらえてdgemm()に渡すときに転置を指定すればいいだけ。行優先なら列方向のstrides[1]が1なら使える。使えるもののうち、転置なら1、転置でないなら0、stridesが飛び飛びで使えないものは-1。
    int is_T() const
    {
        if(strides[0] == 1)
            return 0 ;
        if(strides[1] == 1)
            return 1;
        else
            return -1 ;
    }
    // 列優先なら(1,1), (2,1)と行方向に進むとひとつずつ進むことになる。この1というstridesは変えられない。しかし、(1,1), (1,2)と列方向に進ときは行数分進ことになるが、この数は指定できる。なので基本はstrides[1]をリーディングディメンションに与えることになる。
    // 転置はdataをそのままにstridesとshapeを入れ替えたビューである。3 * 5ならstridesは(1,3)であるところを、転置することで5 * 3の(3,1)にしている。(1,1)から(1,2)はひとつずつ進んで、(1,1)から(2,1)は3つ進ことになる。なので綺麗な行優先の行列である。blasに与えるときはこれを列優先として認識させたうえで、blas内で転置してもらう。なので行優先の行列を転置前の列優先に戻してblasに与えて、CblasTransで転置させればいい。そして戻すのは単純でshapeとstridesを入れ替えればいい。stridesはリーディングディメンションをstrides[1]ではなくstrides[2]を与えればいいだけ。shapeはm,n,kで指定するが、3*5と5*4の行列積で、3*5を転置前のものを与えてCblasTransで転置するとすると、与える行列は5*3と5*4になるが、m,n,kは3,4,5のままである。つまり、転置後のshapeを与えればいいので、そのまま与えればいい。 
    matrix &mul_blas(const matrix &other, matrix &dist) const
    {
        CBLAS_TRANSPOSE trans_a = is_T() ? CblasTrans : CblasNoTrans ; 
        CBLAS_TRANSPOSE trans_b = other.is_T() ? CblasTrans : CblasNoTrans ;
        int lda = is_T() ? strides[0] : strides[1] ;
        int ldb = other.is_T() ? other.strides[0] : other.strides[1] ;
        int ldc = dist.strides[1] ;
        cblas_dgemm
        (
            CblasColMajor, trans_a, trans_b,
            dist.shape[0], dist.shape[1], shape[1],
            1.0,
            data, lda,
            other.data, ldb,
            0.0,
            dist.data, ldc
        ) ;
        return dist ;
    }
    matrix &mul_old(const matrix &other, matrix &dist, int flag = -1) const
    {
        //int max = dist.shape[0] * dist.shape[1] * other.shape[0] ;
        //if(flag == -1)
            //flag = max > 1000000000 ? 1 : 0 ;
        //progress bar(max, "mul_old():") ; 
        for(int i = 0 ; i < dist.shape[0] ; i++)
            for(int j = 0 ; j < dist.shape[1] ; j++)
            {
                double sum = 0 ;  
                for(int k = 0 ; k < other.shape[0] ; k++)
                { 
                    sum += (*this)(i, k) * other(k, j) ;
                    //if(flag)
                        //bar.update(i * dist.shape[1] * other.shape[0] + j * other.shape[0] + k) ;
                }
                dist(i, j) = sum ;
            } 
        return dist ;
    }
    matrix &mul_1(const matrix &other, matrix &dist) const
    {
        int this_st_1 = strides[1] ;
        int other_st_0 = other.strides[0] ;
        for(int i = 0 ; i < dist.shape[0] ; i++)
            for(int j = 0 ; j < dist.shape[1] ; j++)
            {
                double sum = 0 ;
                double *val1 = data + i * strides[0] ;
                double *val2 = other.data + j * other.strides[1] ;
                for(int k = 0 ; k < other.shape[0] ; k++)    
                    sum += *(val1 + k * this_st_1) * *(val2 + k * other_st_0) ;
                dist(i, j) = sum ;
            } 
        return dist ;
    }
    matrix &mul_10(const matrix &other, matrix &dist) const
    {
        int this_st_1 = strides[1] ;
        int other_st_0 = other.strides[0] ;
        for(int i = 0 ; i < dist.shape[0] ; i++)
            for(int j = 0 ; j < dist.shape[1] ; j++)
            {
                double sum = 0 ;
                double *val1 = data + i * strides[0] ;
                double *val2 = other.data + j * other.strides[1] ;
                for(int k = 0 ; k < other.shape[0] / 10 ; k += 10)    
                {
                    sum += *(val1 + k * this_st_1) * *(val2 + k * other_st_0) ;
                    sum += *(val1 + k+1 * this_st_1) * *(val2 + k+1 * other_st_0) ;
                    sum += *(val1 + k+2 * this_st_1) * *(val2 + k+2 * other_st_0) ;
                    sum += *(val1 + k+3 * this_st_1) * *(val2 + k+3 * other_st_0) ;
                    sum += *(val1 + k+4 * this_st_1) * *(val2 + k+4 * other_st_0) ;
                    sum += *(val1 + k+5 * this_st_1) * *(val2 + k+5 * other_st_0) ;
                    sum += *(val1 + k+6 * this_st_1) * *(val2 + k+6 * other_st_0) ;
                    sum += *(val1 + k+7 * this_st_1) * *(val2 + k+7 * other_st_0) ;
                    sum += *(val1 + k+8 * this_st_1) * *(val2 + k+8 * other_st_0) ;
                    sum += *(val1 + k+9 * this_st_1) * *(val2 + k+9 * other_st_0) ;
                }
                dist(i, j) = sum ;
            } 
        return dist ;
    }
    matrix &mul_100(const matrix &mat2, matrix &dist, int pro_flag = 0) const
    {
        matrix tmp = (*this).T() ; // move
        matrix mat1 = tmp ; // copy

        //if(pro_flag == -1)
            //pro_flag = dist.shape[0] * dist.shape[1] > 100000 ? 1 : 0 ;
        //progress bar(dist.shape[0], "mul_100():") ;
       
        for(int i = 0 ; i < dist.shape[0] ; i++)
        { 
            for(int j = 0 ; j < dist.shape[1] ; j++)
            {
                double sum = 0 ; 
                // mat2はoperator()と同じ。mat1は転置しているのでi行を取り出したいならi列を取り出さなければならないのでi * strides[1]
                double *val1 = mat1.data + i * mat1.strides[1] ;
                double *val2 = mat2.data + j * mat2.strides[1] ;
                for(int k = 0 ; k < mat2.shape[0] / 100 ; k+=100)
                {
                    sum += *(val1 + k) * *(val2 + k) ;
                    sum += *(val1 + k+1) * *(val2 + k+1) ;
                    sum += *(val1 + k+2) * *(val2 + k+2) ;
                    sum += *(val1 + k+3) * *(val2 + k+3) ;
                    sum += *(val1 + k+4) * *(val2 + k+4) ;
                    sum += *(val1 + k+5) * *(val2 + k+5) ;
                    sum += *(val1 + k+6) * *(val2 + k+6) ;
                    sum += *(val1 + k+7) * *(val2 + k+7) ;
                    sum += *(val1 + k+8) * *(val2 + k+8) ;
                    sum += *(val1 + k+9) * *(val2 + k+9) ; 
                    sum += *(val1 + k+10) * *(val2 + k+10) ;
                    sum += *(val1 + k+11) * *(val2 + k+11) ;
                    sum += *(val1 + k+12) * *(val2 + k+12) ;
                    sum += *(val1 + k+13) * *(val2 + k+13) ;
                    sum += *(val1 + k+14) * *(val2 + k+14) ;
                    sum += *(val1 + k+15) * *(val2 + k+15) ;
                    sum += *(val1 + k+16) * *(val2 + k+16) ;
                    sum += *(val1 + k+17) * *(val2 + k+17) ;
                    sum += *(val1 + k+18) * *(val2 + k+18) ;
                    sum += *(val1 + k+19) * *(val2 + k+19) ;
                    sum += *(val1 + k+20) * *(val2 + k+20) ;
                    sum += *(val1 + k+21) * *(val2 + k+21) ;
                    sum += *(val1 + k+22) * *(val2 + k+22) ;
                    sum += *(val1 + k+23) * *(val2 + k+23) ;
                    sum += *(val1 + k+24) * *(val2 + k+24) ;
                    sum += *(val1 + k+25) * *(val2 + k+25) ;
                    sum += *(val1 + k+26) * *(val2 + k+26) ;
                    sum += *(val1 + k+27) * *(val2 + k+27) ;
                    sum += *(val1 + k+28) * *(val2 + k+28) ;
                    sum += *(val1 + k+29) * *(val2 + k+29) ;
                    sum += *(val1 + k+30) * *(val2 + k+30) ;
                    sum += *(val1 + k+31) * *(val2 + k+31) ;
                    sum += *(val1 + k+32) * *(val2 + k+32) ;
                    sum += *(val1 + k+33) * *(val2 + k+33) ;
                    sum += *(val1 + k+34) * *(val2 + k+34) ;
                    sum += *(val1 + k+35) * *(val2 + k+35) ;
                    sum += *(val1 + k+36) * *(val2 + k+36) ;
                    sum += *(val1 + k+37) * *(val2 + k+37) ;
                    sum += *(val1 + k+38) * *(val2 + k+38) ;
                    sum += *(val1 + k+39) * *(val2 + k+39) ;
                    sum += *(val1 + k+40) * *(val2 + k+40) ;
                    sum += *(val1 + k+41) * *(val2 + k+41) ;
                    sum += *(val1 + k+42) * *(val2 + k+42) ;
                    sum += *(val1 + k+43) * *(val2 + k+43) ;
                    sum += *(val1 + k+44) * *(val2 + k+44) ;
                    sum += *(val1 + k+45) * *(val2 + k+45) ;
                    sum += *(val1 + k+46) * *(val2 + k+46) ;
                    sum += *(val1 + k+47) * *(val2 + k+47) ;
                    sum += *(val1 + k+48) * *(val2 + k+48) ;
                    sum += *(val1 + k+49) * *(val2 + k+49) ;
                    sum += *(val1 + k+50) * *(val2 + k+50) ;
                    sum += *(val1 + k+51) * *(val2 + k+51) ;
                    sum += *(val1 + k+52) * *(val2 + k+52) ;
                    sum += *(val1 + k+53) * *(val2 + k+53) ;
                    sum += *(val1 + k+54) * *(val2 + k+54) ;
                    sum += *(val1 + k+55) * *(val2 + k+55) ;
                    sum += *(val1 + k+56) * *(val2 + k+56) ;
                    sum += *(val1 + k+57) * *(val2 + k+57) ;
                    sum += *(val1 + k+58) * *(val2 + k+58) ;
                    sum += *(val1 + k+59) * *(val2 + k+59) ;
                    sum += *(val1 + k+60) * *(val2 + k+60) ;
                    sum += *(val1 + k+61) * *(val2 + k+61) ;
                    sum += *(val1 + k+62) * *(val2 + k+62) ;
                    sum += *(val1 + k+63) * *(val2 + k+63) ;
                    sum += *(val1 + k+64) * *(val2 + k+64) ;
                    sum += *(val1 + k+65) * *(val2 + k+65) ;
                    sum += *(val1 + k+66) * *(val2 + k+66) ;
                    sum += *(val1 + k+67) * *(val2 + k+67) ;
                    sum += *(val1 + k+68) * *(val2 + k+68) ;
                    sum += *(val1 + k+69) * *(val2 + k+69) ;
                    sum += *(val1 + k+70) * *(val2 + k+70) ;
                    sum += *(val1 + k+71) * *(val2 + k+71) ;
                    sum += *(val1 + k+72) * *(val2 + k+72) ;
                    sum += *(val1 + k+73) * *(val2 + k+73) ;
                    sum += *(val1 + k+74) * *(val2 + k+74) ;
                    sum += *(val1 + k+75) * *(val2 + k+75) ;
                    sum += *(val1 + k+76) * *(val2 + k+76) ;
                    sum += *(val1 + k+77) * *(val2 + k+77) ;
                    sum += *(val1 + k+78) * *(val2 + k+78) ;
                    sum += *(val1 + k+79) * *(val2 + k+79) ;
                    sum += *(val1 + k+80) * *(val2 + k+80) ;
                    sum += *(val1 + k+81) * *(val2 + k+81) ;
                    sum += *(val1 + k+82) * *(val2 + k+82) ;
                    sum += *(val1 + k+83) * *(val2 + k+83) ;
                    sum += *(val1 + k+84) * *(val2 + k+84) ;
                    sum += *(val1 + k+85) * *(val2 + k+85) ;
                    sum += *(val1 + k+86) * *(val2 + k+86) ;
                    sum += *(val1 + k+87) * *(val2 + k+87) ;
                    sum += *(val1 + k+88) * *(val2 + k+88) ;
                    sum += *(val1 + k+89) * *(val2 + k+89) ;
                    sum += *(val1 + k+90) * *(val2 + k+90) ;
                    sum += *(val1 + k+91) * *(val2 + k+91) ;
                    sum += *(val1 + k+92) * *(val2 + k+92) ;
                    sum += *(val1 + k+93) * *(val2 + k+93) ;
                    sum += *(val1 + k+94) * *(val2 + k+94) ;
                    sum += *(val1 + k+95) * *(val2 + k+95) ;
                    sum += *(val1 + k+96) * *(val2 + k+96) ;
                    sum += *(val1 + k+97) * *(val2 + k+97) ;
                    sum += *(val1 + k+98) * *(val2 + k+98) ;
                    sum += *(val1 + k+99) * *(val2 + k+99) ; 
                }
                dist(i, j) = sum ;
            }
            //if(pro_flag)
                //bar.update(i) ;
        }
        return dist ;
    }
    matrix mul(const matrix &other) const
    {
        matrix result(shape[0], other.shape[1]) ;
        return (*this).mul(other, result) ; 
    }
    friend matrix operator*(const matrix &arg1, const matrix &arg2)
        {return arg1.mul(arg2) ;}

    // mat.mul(3, mat)のように自分を指定すると、(*this)(i,j) = (*this)(i.j) * 3;になり機能する。3パターンのうち自己書き換えとdist書き換えの2パターンに対応。
    matrix &mul(double value, matrix &dist) const 
    {
        for(int i = 0 ; i < dist.shape[0] ; i++) 
            for(int j = 0 ; j < dist.shape[1] ; j++)
                dist(i, j) =  (*this)(i, j) * value ;
        return dist ;
    }
    // 生成。
    friend matrix operator*(const matrix &arg1, double arg2)
    {
        matrix result(arg1.shape[0], arg1.shape[1]) ;
        arg1.mul(arg2, result) ;
        return result ;
    }
    friend matrix operator*(double arg1, const matrix &arg2)
        {return arg2 * arg1 ;}
    matrix &operator*=(const double other)
        {return (*this).mul(other, (*this)) ;}   




    matrix &fill(double value)
    {
        for(int i = 0 ; i < shape[0] ; i++)
            for(int j = 0 ; j < shape[1] ; j++)
                (*this)(i, j) = value ;
        return (*this) ;
    }
    matrix &fill(double (*method)(double))
    {
        for(int i = 0 ; i < shape[0] ; i++)
            for(int j = 0 ; j < shape[1] ; j++)
                (*this)(i, j) = method((*this)(i, j)) ;
        return (*this) ;
    } 
 
    static matrix random
    (
        int rows,
        int cols,
        const file &rand_file=file(file::environ("RINDFILE_PATH") + "/1.bin"),
        double start=-1.0,
        double end=1.0
    ) ;
    
    // 単位行列を生成するクラス関数。行列生成なのでコンストラクタに組み込もうとおもったが、intとstrを受け取って、matrix(3, "identity")とするコンストラクタを作るなら、matrix::identity(3)でいいと思い、クラス関数に。
    static matrix identity(int size)
    {
        matrix result(size, size, 0.0) ;
        for(int i = 0 ; i < result.shape[0] ; i++)
            result(i, i) = 1.0 ;
        return result ;
    }

    // LU分解した行列を生成する関数。5000*5000でinv_oldでは80:03だったが、mulと同じ方法でinvを作ったら23:20に。あと階段上に減っていくので最終的には07:44で計算終わった。
    matrix LU_old() const 
    { 
        if(shape[0] != shape[1])
            throw std::invalid_argument("matrix::LU(): shape[0] != shape[1]") ;  
        matrix result = (*this) ;
        progress bar(shape[0] - 1) ;
        for(int k = 0 ; k < shape[0] - 1 ; k++)
        {
            for(int i = k + 1 ; i < shape[0] ; i++)
            { 
                result(i, k) = result(i, k) / result(k, k) ;
                for(int j = k + 1 ; j < shape[0] ; j++)
                    result(i, j) = result(i, j) - result(i, k) * result(k, j) ;
            }
            bar.update(k) ;
        }
        return result ;
    }
    matrix LU() const 
    { 
        if(shape[0] != shape[1])
            throw std::invalid_argument("matrix::LU(): shape[0] != shape[1]") ;  
        matrix result = (*this) ;
        int st0 = result.strides[0] ;
        int st1 = result.strides[1] ;
        progress bar(shape[0] - 1, "LU():") ;
        for(int k = 0 ; k < shape[0] - 1 ; k++)
        {
            for(int i = k + 1 ; i < shape[0] ; i++)
            { 
                double ik = result(i, k) /= result(k, k) ;
                double *ij = result.data + i * st0 ;
                double *kj = result.data + k * st0 ;
                for(int j = k + 1 ; j < shape[0] ; j++)
                    *(ij + j * st1) -= ik * *(kj + j * st1) ;
            }
            bar.update(k) ;
        }
        return result ;
    }
    
    // a=LU行列とベクトルbを受けって、ax=b, L(Ux)=bを満たすベクトルxを返す、つまり連立方程式を解く関数。数値解析のメモを参照。 lu_mat.linear_system(vec)というようにつかう。
    matrix linear_system(const matrix &vec) const 
    {
        matrix x(shape[0], 1), y(shape[0], 1) ;
        // Ly=b
        for(int i = 0 ; i < shape[0] ; i++)
        {
            y(i, 0) = vec(i, 0) ; 
            for(int j = 0 ; j < i ; j++)
                y(i, 0) = y(i, 0) - (*this)(i, j) * y(j, 0) ;
        }
        // Ux=y
        for(int i = shape[0] - 1 ; i >= 0 ; i--)
        {
            x(i, 0) = y(i, 0) ;
            for(int j = i + 1 ; j < shape[0] ; j++)
                x(i, 0) = x(i, 0) - (*this)(i, j) * x(j, 0) ;
            x(i, 0) = x(i, 0) / (*this)(i, i) ;
        }
        return x ;
    }

    // 逆行列。生成。5000*5000で71:25。さすがにlapackへ。まえはこんなながくなかったはず。
    matrix inv_handmade() const
    {
        matrix result(shape[0], shape[1]) ;
        matrix ide_mat = identity(shape[0]) ;
        matrix lu_mat = (*this).LU() ;
        progress bar(shape[1], "inv():") ;
        for(int i = 0 ; i < shape[1] ; i++)
        {
            result(slice(), i) = lu_mat.linear_system(ide_mat(slice(), i)) ;
            bar.update(i) ;
        }
        return result ;
    }


    matrix inv() const
    {
        matrix result = (*this) ;
        int ipiv[shape[0]] ;
        int info = LAPACKE_dgetrf(LAPACK_COL_MAJOR, shape[0], shape[0], result.data, shape[0], ipiv) ;
        if(info != 0)
            throw std::runtime_error("matrix::inv(): info != 0") ;
        info = LAPACKE_dgetri(LAPACK_COL_MAJOR, shape[0], result.data, shape[0], ipiv);
        if(info != 0)
            throw std::runtime_error("matrix::inv(): info != 0") ;
        return result ;
    }

    // 行数より列数が大きい行列から、疑似逆行列を生成する関数。X^T(XX^T)^{-1}。
    matrix pinv() const
    {
        return (*this).T() * ((*this) * (*this).T()).inv() ;
    }




    double norm() const 
    {
        if(shape[1] != 1)
            throw std::invalid_argument("matrix::norm(): shape[1] != 1") ;
        return calc::sqrt(((*this).T() * (*this))(0,0)) ;
    }


    // eigという名前だが、powermethodをつかって絶対値が最大の固有値ひとつをかえす関数。というか虚数が含まれるのでpower methodはそもそも使えなかった。自作するならQR分解を実装する必要がある。
    double eig() const
        {return (*this).eig((*this).power_method()) ;}
    // 行列とベクトルを受け取って、行列で変換し、前後のノルムの倍率を返す関数。固有ベクトルであればその固有値を返す。
    double eig(const matrix &vec) const
    {
        //std::cout << ((*this) * vec).T() << ", " << vec.norm() << "\n" ;
        //return ((*this) * vec).norm() / vec.norm() ;
        std::cout << "((*this)*vec).norm(): " << ((*this)*vec).norm() << "\n" ;
        return (((*this) * vec).T() * vec)(0,0) / (vec.T() * vec)(0,0) ;
    }
    matrix power_method(const file &rand_file=file(file::environ("RINDFILE_PATH") + "/1.bin")) const 
    { 
        matrix::csr mat((*this)) ;
        matrix vec1(shape[1], 1) ;
        matrix vec2 = matrix::random(shape[1], 1, rand_file) ;
        vec2 = vec2 * (1 / vec2.norm()) ;
        double norm_diff = 1.0 ;
        progress bar(10000000) ;
        while(0.0000001 < norm_diff && norm_diff < 1.999999)
        {
            vec1(slice(), slice()) = vec2 ;
            // vec1 = vec2 ; 速度検証
            mat.mul(vec1, vec2).mul(1 / vec2.norm(), vec2) ;
            norm_diff = (vec2 + vec1.mul(-1, vec1)).norm() ;
            // norm(vec2.add(vec1.mul(-1, vec1), diff_vec)) ; 速度検証
            //bar.update((int)((norm_diff > 1.0 ? norm_diff - 1.0 : 1.0 + -norm_diff) * 10000000)) ;
            std::cout << norm_diff << "\n" ; 
        }
        return vec2 ;
    }

    // lapackを用いてスペクトル半径を求める。
    double max_abs_eig() const
    {
        if(strides[0] != 1 || strides[1] != shape[0])
            throw std::invalid_argument("matrix::max_abs_eig(): strides[0] != 1 || strides[1] != shape[0]") ;
        double wr[shape[0]], wi[shape[0]] ;
        int info = LAPACKE_dgeev(LAPACK_COL_MAJOR,'N','N',shape[0],data,strides[1],wr,wi,NULL,strides[1],NULL,strides[1]) ; 
        if(info != 0)
            throw std::runtime_error("matrix::max_abs_eig(): info != 0") ;
        double result = 0.0 ;
        for(int i = 0 ; i < shape[0] ; i++)
        {
            double abs_eig = calc::sqrt(wr[i] * wr[i] + wi[i] * wi[i]) ;
            if(abs_eig > result)
                result = abs_eig ;
        }
        return result ;
    }






    // 行を指定してその行だけの1次元配列を生成。gnuplot-iostream用。
    std::vector<double> to_vector(int row)
    {
        std::vector<double> result ;
        for(int col = 0 ; col < shape[1] ; col++)
            result.push_back((*this)(row, col)) ;
        return result ;
    }




    // thisが(60,60)であれば、(3,3)とdouble method(matrix)シリーズの関数を受け取って、(3,3)の行列を関数に与えてdoubleに変換することで、(20, 20)の行列を生成して返す関数。
    matrix block(int rows, int cols, double (*method)(const matrix &)) const
    {
        if(shape[0] % rows != 0 || shape[1] % cols != 0)
            throw std::invalid_argument
                ("matrix::block(): shape[0] \% rows != 0 || shape[1] \% cols != 0") ; 
        matrix result(shape[0]/rows, shape[1]/cols) ;
        for(int i = 0 ; i < result.shape[0] ; i++)
            for(int j = 0 ; j < result.shape[1] ; j++)
                result(i, j) = method((*this)({i*rows,(i+1)*rows}, {j*cols,(j+1)*cols})) ;
        return result ;
    }


    // double method(matrix)シリーズ。sum, ave, max, min。
    static double sum(const matrix &mat)
    {
        double sum = 0.0 ;
        for(int i = 0 ; i < mat.shape[0] ; i++)
            for(int j = 0 ; j < mat.shape[1] ; j++)
                sum += mat(i, j) ;
        return sum ;
    }
    double sum() const 
        {return sum(*this) ;}
    // 対角成分の和を返す関数。対角成分はdiagonalというのでdia。
    static double dia(const matrix &mat)
    {
        if(mat.shape[0] != mat.shape[1]) 
            throw std::invalid_argument("matrix::diagonal(): shape[0] != shape[1]") ;
        double sum = 0.0 ;
        for(int i = 0 ; i < mat.shape[0] ; i++)
            sum += mat(i, i) ;
        return sum ;
    }
    // 対角成分をふくまない上三角の部分の和を返す関数。https://manabitimes.jp/math/1332より、LU分解はLower triangular matrix、Upper triangular matrixが元の名前だと思われる。1行2列なら(1,2)を返すが、列数が1だと0.0のままになってしまう。基本は正方行列なので1行1列がそれに該当するのだが、この場合はそのひとつだけある要素を返すようにする。
    static double up(const matrix &mat)
    {
        if(mat.shape[1] == 1)
            return mat(0, 0) ;
        double sum = 0.0 ;
        for(int i = 0 ; i < mat.shape[0] ; i++)
            for(int j = i + 1 ; j < mat.shape[1] ; j++)
                sum += mat(i, j) ;
        return sum ;
    }
    static double low(const matrix &mat)
    {
        if(mat.shape[1] == 1)
            return mat(0, 0) ;
        double sum = 0.0 ;
        for(int i = 0 ; i < mat.shape[0] ; i++)
            for(int j = 0 ; j < i ; j++)
                sum += mat(i, j) ;
        return sum ;
    }
    // lowのうち(1,0),(2,1),...(n,n-1)の一列だけの和を返す関数。sub diagonalより。
    static double sud(const matrix &mat)
    {
        if(mat.shape[1] == 1)
            return mat(0, 0) ;
        double sum = 0.0 ;
        for(int i = 1 ; i < mat.shape[0] ; i++)
            for(int j = i-1 ; j < i ; j++)
                sum += mat(i, j) ;
        return sum ;
    }
    // n行n列なら(n,n),(n-1,n-1),(n,n-1)の3つの和を返す関数。埋め込みでこの3つが高いF値を出していたので、とりあえず作る。1行1列のときは(1,1)を返す。
    static double top(const matrix &mat)
    {
        if(mat.shape[1] == 1)
            return mat(0, 0) ;
        double result = 
            mat(mat.shape[0]-1, mat.shape[1]-1) +
            mat(mat.shape[0]-2, mat.shape[1]-2) +
            mat(mat.shape[0]-1, mat.shape[1]-2) ;
        return result ;
    }
    
    static double max(const matrix &mat)
    {
        double result = mat(0, 0) ;
        for(int i = 0 ; i < mat.shape[0] ; i++)
            for(int j = 0 ; j < mat.shape[1] ; j++)
                if(result < mat(i, j))
                    result = mat(i, j) ;
        return result ;
    }
    static double min(const matrix &mat)
    {
        double result = mat(0, 0) ;
        for(int i = 0 ; i < mat.shape[0] ; i++)
            for(int j = 0 ; j < mat.shape[1] ; j++)
                if(result > mat(i, j))
                    result = mat(i, j) ;
        return result ;
    }

    double mean() const
        {return (*this).sum() / shape[0] / shape[1] ;}
    double var() const
    {
        if(shape[0] != 1)
            throw std::invalid_argument("matrix::var(): shape[0] != 1") ;
        double mean = (*this).mean() ;
        double variance = 0.0 ;
        for(int i = 0 ; i < shape[1] ; i++)
        {
            double deviation = (*this)(0, i) - mean ;
            variance += deviation * deviation ;
        }
        return variance /= shape[1] ;
    }


 
    // 1行のmatrixを二つ受け取って相関係数を求める関数。どちらも1行であることを想定しているが、エラーハンドリングはしていない。行列が渡されたら1行目をつかうことになる。
    static double correlation_coefficient(const matrix &x, const matrix &y)
    {
        if(x.shape[1] != y.shape[1])
            throw std::invalid_argument
                ("matrix::correlation_coefficient(): x.shape[1] != y.shape[1]") ;
        double mean_x = x.mean() ;
        double mean_y = y.mean() ;
        double covariance = 0.0, variance_x = 0.0, variance_y = 0.0 ;
        for(int i = 0 ; i < x.shape[1] ; i++)
        {
            double dx = x(0, i) - mean_x ;
            double dy = y(0, i) - mean_y ;
            covariance += dx * dy ;
            variance_x += dx * dx ;
            variance_y += dy * dy ;
        }
        if(variance_x == 0.0 || variance_y == 0.0)
            throw std::runtime_error
            (
                "matrix::correlation_coefficient(): "
                "variance_x == 0.0 || variance_y = 0.0"
            ) ;
        return covariance / std::sqrt(variance_x * variance_y) ;
    }
    double correlation_coefficient(const matrix &other) const
        {return correlation_coefficient((*this), other) ;}

    matrix autocorrelation() const 
    {
        matrix result(shape[1], 1) ;
        double mean = (*this).mean() ;
        double variance = (*this).var() ;
        if(variance == 0.0)
            throw std::runtime_error("matrix::autocorrelation(): variance == 0.0") ;
        for(int i = 0 ; i < shape[1] ; i++)
        {
            double autocovariance = 0.0 ;    
            for(int j = 0 ; j < shape[1] - i ; j++)
                autocovariance += ((*this)(0, j) - mean) * ((*this)(0, j + i) - mean) ;
            autocovariance /= (shape[1] - i) ; 
            result(i, 0) = autocovariance / variance ;
        }
        return result ;
    }


    





    // doubleを受け取って、その値より大きい要素を1、以下の要素を0としたmatrixを生成して返す関数。多数決で使う。
    matrix above(double border) const
    {
        matrix result(shape[0], shape[1]) ;
        for(int j = 0 ; j < result.shape[1] ; j++)
            for(int i = 0 ; i < result.shape[0] ; i++)
                result(i, j) = (*this)(i, j) > border ? 1 : 0 ;
        return result ;
    }



    // valueを受け取ってその値をもつ要素の数を返す関数。flagが0になるとvalueではない要素の数を返すようにする。(0, false)で非ゼロを数える。
    // doubleとdoubleに==は使えないのかもと思ったがちゃんと使える。cppだけだっけ。後で調べる。
    int count(double value, int flag=1) const
    {
        int result = 0 ;
        for(int i = 0 ; i < shape[0] ; i++)
            for(int j = 0 ; j < shape[1] ; j++)
                if
                (
                    (flag != 0 && (*this)(i, j) == value) ||
                    (flag == 0 && (*this)(i, j) != value)
                )
                    result += 1 ;
        return result ;
    }


    class csr
    {
    public:
        int *shape ;
        int len ;
        double *data ;
        int *indices ;
        int *indptr ;
        
        // NULL入れないと、~csrでのfree()でエラーがでる。
        csr() : shape(NULL), data(NULL), indices(NULL), indptr(NULL) {}
        csr(const matrix &input)
        {
            shape = (int *)malloc(sizeof(int) * 2) ;
            shape[0] = input.shape[0] ;
            shape[1] = input.shape[1] ;
            len = input.count(0, false) ;
            data = (double *)malloc(sizeof(double) * len) ;
            indices = (int *)malloc(sizeof(int) * len) ;
            indptr = (int *)malloc(sizeof(int) * (shape[0] + 1)) ;
        
            int index = 0 ; 
            for(int i = 0 ; i < shape[0] ; i++)
            {
                indptr[i] = index ;
                for(int j = 0 ; j < shape[1] ; j++)
                    if(input(i, j) != 0)
                    {
                        data[index] = input(i, j) ;
                        indices[index] = j ;
                        index++ ;
                    }
            }
            indptr[shape[0]] = index ;
        }
        // ファイルストリームのラッパークラスであるfileを受け取ってそこから読み取る。file(char *, mode="r")のコンストラクタにより、csr(file)がcsr(char*)に対応。generated:というログ出力も必要ないのでcsr(file)だけで二つに対応できる。
        csr(const file &input_file)
        { 
            shape = (int *)malloc(sizeof(int) * 2) ;
            input_file.read(shape, sizeof(int) * 2) ;
            input_file.read(&len, sizeof(int)) ;
            data = (double *)malloc(sizeof(double) * len) ;
            input_file.read(data, sizeof(double) * len) ;
            indices = (int *)malloc(sizeof(int) * len) ;
            input_file.read(indices, sizeof(int) * len) ;
            indptr = (int *)malloc(sizeof(int) * (shape[0] + 1)) ;
            input_file.read(indptr, sizeof(int) * (shape[0] + 1)) ;  
        }  

        ~csr()
        {
            //std::cout << "~csr():" << (void *)this << std::endl ;
            free(shape) ;
            free(data) ;
            free(indices) ;
            free(indptr) ;
        }


        // esn.hppの{csr w_x;w_x = csr(input_file);}用。ムーブ。
        csr &operator=(csr &&other)
        {
            //std::cout << "csr::operator=(csr &&): " << (void *)this << " <- " << (void *)&other << "\n" ;
            (*this).~csr() ;
            shape = other.shape ;
            len = other.len ;
            data = other.data ;
            indices = other.indices ;
            indptr = other.indptr ;
            other.shape = NULL ;
            other.data = NULL ;
            other.indices = NULL ;
            other.indptr = NULL ;
            return (*this) ;
        }

        

        const csr &write(const file &output_file) const
        { 
            output_file.write(shape, sizeof(int) * 2) ;
            output_file.write(&len, sizeof(int)) ;
            output_file.write(data, sizeof(double) * len) ;
            output_file.write(indices, sizeof(int) * len) ;
            output_file.write(indptr, sizeof(int) * (shape[0] + 1)) ; 
            return (*this) ; 
        }
        const csr &write(const str &file_name) const
        { 
            (*this).write(file(file_name, "x")) ;
            std::cout << "generated: " << file_name << "\n" ;
            return (*this) ; 
        }
        // 上二つだけだと、write(char *)が呼ばれたときに、file(char *)も、str(char *)もあり、file a = char *, str a = char *もできてしまうため、どっちを呼び出せばいいかコンパイラが判定できず、オーバーロードが上手くいかない。write(char *)はwrite(str)を呼ぶようにして、write(file)はwrite(file)でしか呼べないようにしたい。そこで、write(char *)をオーバーロードすればすべて解決する。
        const csr &write(const char *file_name) const
            {return (*this).write(str(file_name)) ;}
        
        // つかわないかも。
        double operator()(int row , int col) const
        {
            for(int i = indptr[row] ; i < indptr[row + 1] ; i++)
                if(indices[i] == col)
                    return data[i] ;
            return 0 ;
        } 

        // csrのrはrowで、行ごとのアクセスを最適化したものなので、疎 * 密限定である。もし、密 * 疎をしたいのならcscを作る必要あり。いまはcsrでいいので必要になったら作ろう。なので、csrではcsr * matだけ定義すればいい。
        matrix &mul(const matrix &other, matrix &dist) const
        {
            for(int i = 0 ; i < dist.shape[0] ; i++)
                for(int j = 0 ; j < dist.shape[1] ; j++)
                {
                    double sum = 0 ;
                    for(int k = indptr[i] ; k < indptr[i + 1] ; k++)
                        sum += data[k] * other(indices[k], j) ;
                    dist(i, j) = sum ;
                }
            return dist ;
        }
        matrix mul(const matrix &other) const
        {
            matrix result(shape[0], other.shape[1]) ;
            return (*this).mul(other, result) ;
        }
        friend matrix operator*(const matrix::csr &arg1, const matrix &arg2)
            {return arg1.mul(arg2) ;}

        // 自己書き換え型のcsr * double。
        csr &operator*=(double value)
        {
            for(int i = 0 ; i < len ; i++)
                data[i] = data[i] * value ;
            return (*this) ;
        } 
 
        csr &randomize_nonzero
        ( 
            const file &rand_file=file(file::environ("RINDFILE_PATH") + "/1.bin"),
            double start=-1.0,
            double end=1.0
        ) ; 

        matrix to_matrix() const
        {
            matrix result(shape[0], shape[1], 0.0) ;
            for(int i = 0 ; i < shape[0] ; i++)
                for(int k = indptr[i] ; k < indptr[i + 1] ; k++)
                    result(i, indices[k]) = data[k] ;
            return result ;
        }
    } ;

} ;

#endif

