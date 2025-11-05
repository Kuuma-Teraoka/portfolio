
#ifndef _esn_hpp
#define _esn_hpp

/*
　w_in, w_x, x0の3つのパラメータを生成するのをコンストラクタにして、
echo_state_network model(...)
　でインスタンス変数に保存する。そして、
model.train(u, d)
　で学習してw_outを保存。そして、
model(u)
　と時系列を渡すと学習済みのw_outで同じ長さの出力を返す。

　問題はリザバーの状態ベクトルの時系列であるxを保存するかどうか。学習後に時系列を与える場合も、x0は所詮乱数なので、x(last)、つまり現在のリザバーの状態ベクトルを使えばいいと思う。espを満たしていれば変わらない。
　xの最後以外は学習でしか使わない。学習はtrain(u, d)でuとdをセットで行うので、xを所持する必要はないと思う。

　feedbackはxの最後からyを作り、それを入力すればいいので、やはり現在のx(t)とw_in, w_x, w_outだけあれば、
model.feedback(5000)
　とすれば長さ5000の予測時系列を生成する。

matrix::csr w_x
　を
matrix w_x
　に変えても、matrixもmatrix::csrもrandomize_nonzeroがあるので互換性がある。そして速度は、
input(): 272/s eta 01:33/03:40
input(): 4/s eta 00:56/235:22
　という感じ。70倍ちがう。
*/

#include "matrix.hpp"
#include "network.hpp"
#include "progress.hpp"

class echo_state_network
{
public:
    matrix w_in ;
    matrix::csr w_x ;
    matrix w_out ;
    matrix x ;        

    echo_state_network
    (
        int Nu, 
        double range_w_in, 
        int Nx, 
        const matrix &neuron_links,
        double spectral_radius=0.9,
        const file &rand_file=file(file::environ("RINDFILE_PATH") + "/1.bin")
    ):
        w_in(matrix::random(Nx, Nu, rand_file, -range_w_in, range_w_in)),
        x(matrix::random(Nx, 1, rand_file)),
        w_x(neuron_links)
    {
        w_x.randomize_nonzero(rand_file) ;
        std::cout << "w_x.max_abs_eig(): calculate..." << std::flush ;
        progress time ;
        w_x *= spectral_radius / w_x.to_matrix().max_abs_eig() ;
        //w_x *= w_x.to_matrix().max_abs_eig() ;
        std::cout << "complete in " << time.elapsed() << " [sec]" << "\n" ;
    }

    // キーワード引数みたいに渡せる。cpp.txt参照。
    struct constructor_args
    {
        int Nu ;
        double range_w_in ;
        int Nx ;
        const matrix &neuron_links ;
        double spectral_radius = 0.9 ;
        const file &rand_file = file(file::environ("RINDFILE_PATH") + "/1.bin") ;
    } ;
    echo_state_network(const constructor_args& args): echo_state_network
    (
        args.Nu, 
        args.range_w_in, 
        args.Nx,
        args.neuron_links, 
        args.spectral_radius,
        args.rand_file
    ) {}

    echo_state_network(const str &file_name)
    {
        file input_file(file_name) ;
        w_in = matrix(input_file) ;  
        w_x = matrix::csr(input_file) ;
        w_out = matrix(input_file) ;
        x = matrix(input_file) ;
    }

    // デストラクタは必要なし。デストラクタ定義済みのmatrixでしか構成されていないので。
    //~echo_state_network()
    

    // 1つの.esnファイルにw_in, w_x, w_out, xの順にファイルに書き出す。matはshapeから、csrはshapeとlenからバイト長さを判別できるようになっているので問題ない。
    const echo_state_network &write(const str &file_name) const
    {
        file output_file(file_name, "x") ;
        w_in.write(output_file) ;
        w_x.write(output_file) ;
        w_out.write(output_file) ;
        x.write(output_file) ;
        std::cout << "generated: " << file_name << "\n" ;
        return (*this) ;
    }

    // x=x0 から、x=(x1,x2,...,x60000)に。x0は捨てる。
    echo_state_network &input(const matrix &u)
    { 
        matrix a = w_in * u(0) ;
        matrix b = w_x * x ; 
        x = matrix(x.shape[0], u.shape[1]) ;
        x(0) = a.add(b, a).fill(std::tanh) ;  
        for(int i = 1 ; i < x.shape[1] ; i++)
        {
            // 上は03:40で、下は03:58。あまり変わらないかも。-O3ありだっけ。
            x(i) = w_in.mul(u(i), a).add(w_x.mul(x(i-1), b), a).fill(std::tanh) ;
            //x(i) = (w_in * u(i) + w_x * x(i-1)).fill(std::tanh) ;
        }
        return (*this) ;
    }

    // w_outだけ書き換え
    echo_state_network &ridge_regression(const matrix &d, double beta=0.0001)
    {
        matrix i = matrix::identity(x.shape[0]) * beta ;
        w_out = d * x.T() * (x * x.T() + i).inv() ;  
        return (*this) ;
    }

    // xとw_outからyを求めて返す関数。過去のxはもう使わないのでx=(x1,x2,...,x60000)からx=(x60000,)に。
    matrix output()
    {
        if(w_out.data == NULL)
            throw std::invalid_argument
                ("echo_state_network::output(): w_out.data == NULL") ;
        matrix result = w_out * x ;  
        x = x(x.shape[1] - 1).copy() ; 
        return result ;
    }

    // yを返す。trainに関してはyがいらない場合もある。ただ、yをインスタンス変数に保存することはしたくないので、戻り値として返すして、保存しないようにする。
    matrix train(const matrix &u, const matrix &d, double beta=0.0001)
    {
        std::cout << "esn.train(): calculate..." << std::flush ;
        progress time ;
        matrix result = (*this).input(u).ridge_regression(d, beta).output() ;
        std::cout << "complete in " << time.elapsed() << " [sec]" << "\n" ;
        return result ;
    }
    matrix operator()(const matrix &u)
        {return (*this).input(u).output() ;}

    matrix feedback(int time)
    {
        matrix result(w_out.shape[0], time) ;
        result(0) = (*this)((*this).output()) ;
        for(int i = 1 ; i < time ; i++)
            result(i) = (*this)(result(i - 1)) ;
        return result ;
    }

    static int t_valid(const matrix &output, const matrix &pred, double e = 1)
    { 
        double denom = 0 ;
        for(int i = 0 ; i < pred.shape[1] ; i++)
            denom += (pred(i) * pred(i).T())(0, 0) ;
        denom = std::sqrt(denom / pred.shape[1]) ;
        for(int i = 0 ; i < pred.shape[1] ; i++)
        {
            std::cout << (pred(i) - output(i)).norm() / denom << "\n" ;
            if((pred(i) - output(i)).norm() / denom > e)
                return i ;
        }
        return -1 ;
    }
    int t_valid(const matrix &pred, double e = 1)
        {return (*this).t_valid((*this).feedback(pred.shape[1]), pred, e) ;} 

} ;

#endif

