
#ifndef _network_hpp
#define _network_hpp

/*
　まず、WSモデルとBAモデルの前にデータ構造について整理する。ネットワークのデータ構造として辺リスト、隣接リスト、隣接行列の3つが考えられる。

　辺リストは
[(1, 2), (1, 3), (2, 3), (2, 4)]
　というようなint2つのtupleを要素に持つlist。cppならstd::vector<std::tuple<int, int>>。
　重みありなら
[(1, 2, 3.5), (1, 3, -2.4), (2, 3, 2.1), (2, 4, 4.0)]
　のようなint, int, floatのtupleになる。重みなしで方向なしならset<set<int, int>>でいいかも。set.hppやcpp.txtのunordered_setに書いているが、上手くいかないのでvectorに。

　隣接リストは
{1: [3, 4], 2: [1, 3], 3: [1, 2], 4: [1, 2, 3]}
　というようなintと、intのlistのペアを要素に持つdict。ただ、重みを保存するのはめんどくさい。csr形式と似ているのでそっちでいいかも。ただ、可変ではない。

　隣接行列は自作クラスであるmatrixをつかう。a_klであればlからkへの有向辺があるときに1になるようになっている。つまり、1 0という辺があれば、[0, 1]を1にしている。
　これは逆の方がいいのではと一瞬思ったが、リカレント結合重み行列Wにxをいれたときに、Wの一行目にたいして、
x0' = w_00x0 + w_01x1 + ... 
　となっている。x1からx0に有向辺が伸びているときに、w_01が重みとなってかかっている。よって、ネットワークに対して、それぞれの頂点にいまの状態を付加して、そのベクトルを考えたときに、Wxとすると、ネットワークの辺の重みに対して、xが更新されるが、Wは逆でないと、このように行列積をかけない。
　つまり、隣接行列を作るときは、2から1なら、2行1列でなく、1行2列にしておいたほうがいい。

*/

#include <unordered_set>

#include "matrix.hpp"
#include "file.hpp"
#include "randgen.hpp"
#include "progress.hpp"

namespace network
{
    class edge_list
    {
    public:
        std::vector<std::vector<int>> data ;
        int node ;

        edge_list(int node): node(node) {}

        auto begin() {return data.begin() ;}
        auto begin() const {return data.begin() ;}
        auto end() {return data.end() ;}
        auto end() const {return data.end() ;}
        auto append(std::initializer_list<int> args) {return data.push_back(args) ;}
        auto push_back(std::initializer_list<int> args) {return data.push_back(args) ;}

        matrix to_matrix(const randgen &rangen=randgen())
        {
            matrix result(node, node, 0) ;
            for(auto &i : (*this))
            {
                if(rangen.float64(0.0, 1.0) < 0.5)
                    result(i[0], i[1]) = 1 ;
                else
                    result(i[1], i[0]) = 1 ;
            }
            return result ;
        }

        friend std::ostream& operator<<(std::ostream& os, const edge_list& input)
        {
            for(auto &i : input)
                std::cout << i[0] << " " << i[1] << "\n" ;
            return os ;
        }
    } ;

    const str rand_dir = file::environ("RINDFILE_PATH") ;

    // ランダムネットワークを隣接行列matrixで生成する関数。隣接リスト、辺リスト、csrなどあるが、あとで整理する。自己ループなし。辺向きあり。
    matrix random(int nodes=20, int edges=100, const file &rand_file=file(rand_dir + "/1.bin"))
    {  
        if(edges > nodes * nodes - nodes)
            throw std::invalid_argument
                ("network::random(): edges > nodes * nodes - nodes") ;
        matrix result(nodes, nodes, 0.0) ;
        int i = 0 ;
        while(i < edges)
        {
            int rand_value = randgen::uli(rand_file) % (nodes * nodes) ;
            int row = rand_value / nodes ;
            int col = rand_value % nodes ;
            if(result(row, col) == 0 && row != col)
            {
                result(row, col) = 1 ;
                i += 1 ;
            }
        }
        return result ;
    }


    // 円環格子のネットワークを辺リストstd::vector<std::tuple<int, int>>で生成する関数。kが円環格子グラフの各頂点の次数。ただ、偶数が前提なのでkは1,2,...を受け取るようにして、k*2が次数になるようにする。node=4,k=1なら、[(0,1),(1,2),(2,3),(3,0)]を生成する。0からカウントすることに注意。
    edge_list lattice(int node = 20, int k = 2)
    {
        edge_list result(node) ;
        for(int i = 0 ; i < node ; i++)
            for(int j = 1 ; j <= k ; j++)
                result.data.push_back({i, (i + j) % node}) ; 
        return result ;
    }
    /*
    　円環格子からWSモデルを隣接行列で生成する関数。複雑ネットワーク.txt参照。といっても、それぞれの辺に対して確率で繋ぎかえるだけ。ただし、自己ループはなしなので注意。
    　ここで、WSモデルは辺に向きがないので隣接行列にする際に乱数で50%で向きを決める。また、つなぎ変えた先が他の辺に重なること、自己ループを上手く回避している。 
    　他の辺に重なるかどうか判定する際に{1,2}と{2,1}の両方を検索する必要がある。setを自作できればこれも現実的になるが、とりあえず隣接行列に変換しながら繋ぎかえるようにした。
    　ちなみに辺の本数はnode*kになる。
    */
    matrix watts_strogatz
    ( 
        int node,
        const randgen &rangen=randgen(), 
        int k = 5,
        double p = 0.01
    )
    {
        matrix result(node, node, 0) ;
        auto base = lattice(node, k) ;
        for(const auto &i : base)
            result(i[0], i[1]) = 1 ;
        for(auto &i : base)
        {
            if(rangen.float64(0.0, 1.0) < p)
            {
                result(i[0], i[1]) = 0 ;
                do
                {
                    i[0] = rangen.uint64() % node ;
                    i[1] = rangen.uint64() % node ;
                } while(i[0] == i[1] || result(i[0], i[1]) == 1) ;
                result(i[0], i[1]) = 1 ;
            }
            else
                if(rangen.float64(0.0, 1.0) < 0.5)
                {
                    result(i[0], i[1]) = 0 ;    
                    result(i[1], i[0]) = 1 ;
                }
        }
        return result ;
    } 

 

    // deg_lst / sum(deg_lst)でint配列をfloat64配列にして返したい。list<T>の自作をしようと思ったが、numpy.ndarrayは行列を多次元に拡張することにより、1次元配列としても使えるようになっている。matrix.hppをmatrix<int>というようにデータ型に対応できるようにした上でndimを実装すればlistも必要ない。このある種の革命のような発想こそndarrayの真の価値なのだろう。だが、いやだからこそこれをcppで自作したい。とりあえずいまのmatrixを使うことにする。
    // 頂点数は(6, 8, 20)で20になる。1+2+...+7+6*12=100
    edge_list barabasi_albert(int degree, int init_node, int node, const randgen &rangen=randgen())
    {
        if(degree > init_node)
            throw std::invalid_argument("network::barabasi_albert(): degree > init_node") ;
        edge_list result(node) ;
        for(int i = 1 ; i < init_node ; i++)
            for(int j = 0 ; j < i ; j++)
                result.push_back({i, j}) ; // complete_graph
        matrix deg_lst(node, 1, 0) ;
        for(int i = 0 ; i < init_node ; ++i)
            deg_lst(i, 0) = init_node - 1 ; 
        //progress bar(node) ;
        for(int i = init_node ; i < node ; ++i)
        {
            std::vector<int> selected(degree, 0) ;
            while(std::unordered_set<int>(selected.begin(), selected.end()).size() < (size_t)degree)
                for(auto &j : selected)
                    j = rangen.roulette(deg_lst * (1 / deg_lst.sum())) ;
            for(auto &j : selected)
                {result.push_back({i, j}) ; deg_lst(j, 0) += 1 ;}
            deg_lst(i, 0) = degree ;
            //bar.update(i) ;
        }
        return result ;
    }







    





}




#endif

