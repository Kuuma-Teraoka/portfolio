
/*
　元となる時系列r(r=1,2,...20)のうちそれぞれに対して、dimensionを2,3,...、delayを1,2,...、と変化させて、7*10くらいの時系列ができる。また、これに、状態変数ごとに自己相関が0になるdelayを採用して変化させたものも加える。なので、元になる時系列ひとつに対して、7*(10+1)の時系列ができる。そして、それぞれに対して、10個くらいのリザバーで推定。
　状態変数のそれぞれの自己相関を出力したい。自己相関自体は、dimensionに関係ないので、時系列ひとつに対して状態変数の数しかできない。ファイル名に、最初に0になるときの値、つまり採用したdelayをつけて、autocorrelation_1_368.fmatのように出力。自己相関によって変化させた時系列の長さはこのうちの最大値*demensionだけ小さくなっているはず。
　r=1,2,...,20のディレクトリにtruth_links.fmat, original_ts.fmatと、autocorrelation_1_368.fmatたちを作る。そして、さらに3_50というような、{dimension}_{delay}というディレクトリを作り、中にts.fmatとリザバー10個と推定結果、多数決結果を出力。
　リザバー10個の箇所が冗長だったので関数にした。できれば時系列のforループをつくりたかったが、元となる時系列、自己相関の時系列*3~7と、delay10~300*3~7をすべてひとつのforループにするのは不可能なので。

　リザバーの初期化の20sを短縮するために最初に作っておいて、それを読み込むようにしようと思ったのだが、w_inはdimによって大きさが変わるのでやめた。

*/

#include <unistd.h> // getpid()
#include <vector>
#include <set>

#include "str.hpp"
#include "file.hpp"
#include "matrix.hpp"
#include "network.hpp"
#include "coupled_lorenz.hpp"
#include "coupled_rossler.hpp"
#include "echo_state_network.hpp"
#include "infer_links.hpp"
#include "notify.hpp"
#include "progress.hpp" // autocorの計算が長いので

void analyze(const matrix &truth_links, const matrix &ts, int dim, str dist_dir) ;
void simulation(int r) ;

const str rand_dir = file::environ("RINDFILE_PATH") ;

int ensemble = 100 ;
std::set<int> valid_ids = {3, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100} ;
int node = 20 ;
int edge = 100 ;
double h_value = 0.06 ;
double c = 0.3 ;
double tau = 0.02 ;
int dump = 20000 ;
int learn = 60000 ;
int pred = 10000 ; 
double beta = 0.0001 ;




int main()
{
    try
    { 
        for(int r = 0 ; r < 20 ; ++r)
        {
            simulation(r) ; 
            notify::send(str(getpid()) + " completed rand: " + str(r)) ;
        }
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << "\n" ;
        notify::send(e.what()) ;
        return 1 ;
    }
    return 0 ;
}





void simulation(int r)
{
    const str dist_dir = str(r) ; 
    file::makedir(dist_dir) ;
    try
        {file(dist_dir + "/lock.bin", "x") ;}
    catch(const file::file_exists_error& e)
    {
        std::cout << dist_dir << ": already calcurated\n" ;
        return ;
    }
    
    

    
 
    
    matrix truth_links = network::random(node, edge, rand_dir + "/" + str(r) + ".bin") ;
    //matrix truth_links = network::watts_strogatz(node, rand_dir + "/" + str(r) + ".bin", edge / node, 0.01) ; 
    //matrix truth_links = network::barabasi_albert(6, 8, 20, rand_dir + "/" + str(r) + ".bin").to_matrix(rand_dir + "/" + str(r) + ".bin") ; 
    truth_links.write(dist_dir + "/truth_links.fmat") ;

    matrix original_ts = coupled_lorenz::time_series
    (
        truth_links,
        dump+learn+pred,
        rand_dir + "/" + str(r) + ".bin",
        h_value,
        c,
        tau
    ) ;
    original_ts.write(dist_dir + "/ts_original.fmat") ;
    original_ts = original_ts({0,-1,3}, {dump, dump+learn+1}).copy() ; 

    /*matrix original_ts = coupled_rossler::time_series 
    (
        truth_links, 
        learn+1, 
        rand_dir + "/" + str(r) + ".bin", 
        {0.8, 1.2}, 
        0.1, 
        tau
    ) ;
    original_ts.write(dist_dir + "/ts_original.fmat") ;
    original_ts = original_ts({0,-1,3}, {0, learn+1}).copy() ;*/


    /*
    // autocorrelationを求めて出力し、delay_lstを作る。
    progress time(100, "autocorrelation: calculate...") ; 
    std::vector<int> auto_delay(original_ts.shape[0]) ;
    for(int i = 0 ; i < original_ts.shape[0] ; i++)
    {
        matrix auto_cor = original_ts(i, slice()).autocorrelation() ;
        for(int j = 0 ; j < original_ts.shape[1] ; j++)
            if(auto_cor(j, 0) < 0)
            {
                auto_delay[i] = j ;
                auto_cor.write
                    (dist_dir + "/autocorrelation_" + str(i) + "_" + str(j) + ".fmat") ;
                break ;
            }
    }
    std::cout << "complete in " << time.elapsed() << " [sec]" << "\n" ;
    */

    analyze(truth_links, infer_links::delayed_embedding(original_ts, 3, 2), 3, dist_dir + "/" + str(3) + "_" + str(2)) ;  
    analyze(truth_links, original_ts, 1, dist_dir + "/" + str(1) + "_" + str(0)) ;

    /*
    for(int dim : {2,3,4,5,6,7,8,9})
    {
        analyze
        (
            truth_links,
            infer_links::delayed_embedding(original_ts, dim, auto_delay),
            dim, 
            dist_dir + "/" + str(dim) + "_auto"
        ) ;

        for(int delay : {1,2,3,4,5,6,7,8,9})
        {
            analyze
            (
                truth_links,
                infer_links::delayed_embedding(original_ts, dim, delay),
                dim,
                dist_dir + "/" + str(dim) + "_" + str(delay)
            ) ;  
        }
    }
    */ 
 
    


    
}





void analyze(const matrix &truth_links, const matrix &ts, int dim, str dist_dir)
{
    file::makedir(dist_dir) ;

    ts.write(dist_dir + "/ts.fmat") ;

    matrix sum_sum(node, node, 0.0), sum_dia(node, node, 0.0), sum_max(node, node, 0.0), sum_min(node, node, 0.0), sum_low(node, node, 0.0), sum_sud(node, node, 0.0), sum_top(node, node, 0.0) ;
    std::vector<matrix> sum(dim * dim, matrix(node, node, 0.0)) ;
    std::vector<matrix> sum_t(dim * dim, matrix(node, node, 0.0)) ; 

    for(int id = 0 ; id < ensemble ; id++)
    {
        echo_state_network model
        ({
            .Nu = node * dim,
            .range_w_in = 0.1,
            .Nx = 5000,
            .neuron_links = network::random
                (5000, 5000 * 3, rand_dir + "/" + str(id) + ".bin"),
            .spectral_radius = 0.9,
            .rand_file = rand_dir + "/" + str(id) + ".bin"
        }) ;
        model.train(ts({0, ts.shape[1]-1}), ts({1, -1}), beta) ;
        matrix stcd = infer_links::stcd(model, ts({0, ts.shape[1]-1}), tau) ;

        model.write(dist_dir + "/model_" + str(id) + ".esn") ; 
        stcd.write(dist_dir + "/stcd_" + str(id) + ".fmat") ; 




        matrix inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(dim, dim, matrix::sum), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_sum_" + str(id) + ".fmat") ;
        sum_sum += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(dim, dim, matrix::dia), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_dia_" + str(id) + ".fmat") ; 
        sum_dia += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(dim, dim, matrix::max), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_max_" + str(id) + ".fmat") ; 
        sum_max += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(dim, dim, matrix::min), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_min_" + str(id) + ".fmat") ;
        sum_min += inferred_links ;

        // なぜかこれを実行すると、double freeになる。ほんとになんで。matrix:upをmatrix:minにしたら動く。さらに上のをmatrix:minからmatrix:upにしても動く。もう意味をなしていない。いや、範囲外メモリアクセスが起きていて、エラー検知が弱かっただけ。sanitizerでbaysianが原因とわかった。そして、原因はしょうもない。upはすべて0になって最大値とれずに(-1,-1)のままになってしまうというだけ。ほんとにくだらない。
        // upはよく考えたらx_j(t+6\tau) -> x_i(t+5\tau)のstcdで、x_j(t+5\tau) -> x_i(t+6\tau)とかはlowだった。lowにする。
        inferred_links = infer_links::bayesian(stcd.block(dim, dim, matrix::low), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_low_" + str(id) + ".fmat") ;
        sum_low += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(dim, dim, matrix::sud), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_sud_" + str(id) + ".fmat") ;
        sum_sud += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(dim, dim, matrix::top), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_top_" + str(id) + ".fmat") ;
        sum_top += inferred_links ;

        for(int i = 0 ; i < dim ; i++)
            for(int j = 0 ; j < dim ; j++)
            {
                inferred_links = infer_links::bayesian
                    (stcd({i,-1,dim}, {j,-1,dim}), edge) ;
                infer_links::confusion_matrix(truth_links, inferred_links).write
                    (dist_dir + "/confusion_mat_" + str(i) + str(j) + "_" + str(id) + ".fmat") ;
                sum[i*dim + j] += inferred_links ;
            }

        for(int i = 0 ; i < dim ; i++)
            for(int j = 0 ; j < dim ; j++)
            {
                inferred_links = infer_links::bayesian
                    (stcd.T()({i,-1,dim}, {j,-1,dim}), edge) ;
                infer_links::confusion_matrix(truth_links, inferred_links).write
                    (dist_dir + "/confusion_mat_t" + str(i) + str(j) + "_" + str(id) + ".fmat") ;
                sum_t[i*dim + j] += inferred_links ;
            }






        if(valid_ids.count(id + 1))
        {
            sum_sum.write(dist_dir + "/sum_sum_" + str(id + 1) + ".fmat") ;
            infer_links::confusion_matrix(truth_links, sum_sum.above((id + 1) / 2)).write
                (dist_dir + "/confusion_mat_total_sum_" + str(id + 1) + ".fmat") ;

            sum_dia.write(dist_dir + "/sum_dia_" + str(id + 1) + ".fmat") ;
            infer_links::confusion_matrix(truth_links, sum_dia.above((id + 1) / 2)).write
                (dist_dir + "/confusion_mat_total_dia_" + str(id + 1) + ".fmat") ;

            sum_max.write(dist_dir + "/sum_max_" + str(id + 1) + ".fmat") ;
            infer_links::confusion_matrix(truth_links, sum_max.above((id + 1) / 2)).write
                (dist_dir + "/confusion_mat_total_max_" + str(id + 1) + ".fmat") ;

            sum_min.write(dist_dir + "/sum_min_" + str(id + 1) + ".fmat") ;
            infer_links::confusion_matrix(truth_links, sum_min.above((id + 1) / 2)).write
                (dist_dir + "/confusion_mat_total_min_" + str(id + 1) + ".fmat") ;

            sum_low.write(dist_dir + "/sum_low_" + str(id + 1) + ".fmat") ;
            infer_links::confusion_matrix(truth_links, sum_low.above((id + 1) / 2)).write
                (dist_dir + "/confusion_mat_total_low_" + str(id + 1) + ".fmat") ;

            sum_sud.write(dist_dir + "/sum_sud_" + str(id + 1) + ".fmat") ;
            infer_links::confusion_matrix(truth_links, sum_sud.above((id + 1) / 2)).write
                (dist_dir + "/confusion_mat_total_sud_" + str(id + 1) + ".fmat") ;

            sum_top.write(dist_dir + "/sum_top_" + str(id + 1) + ".fmat") ;
            infer_links::confusion_matrix(truth_links, sum_top.above((id + 1) / 2)).write
                (dist_dir + "/confusion_mat_total_top_" + str(id + 1) + ".fmat") ;

            for(int i = 0 ; i < dim ; i++)
                for(int j = 0 ; j < dim ; j++)
                {
                    sum[i*dim + j].write(dist_dir + "/sum_" + str(i) + str(j) + "_" + str(id + 1) + ".fmat") ;
                    infer_links::confusion_matrix(truth_links, sum[i*dim + j].above((id + 1) / 2)).write(dist_dir + "/confusion_mat_total_" + str(i) + str(j) + "_" + str(id + 1) + ".fmat") ;
                }
            for(int i = 0 ; i < dim ; i++)
                for(int j = 0 ; j < dim ; j++)
                {
                    sum_t[i*dim + j].write(dist_dir + "/sum_t" + str(i) + str(j) + "_" + str(id + 1) + ".fmat") ;
                    infer_links::confusion_matrix(truth_links, sum_t[i*dim + j].above((id + 1) / 2)).write(dist_dir + "/confusion_mat_total_t" + str(i) + str(j) + "_" + str(id + 1) + ".fmat") ;
                }
        }

        
    } 


}








