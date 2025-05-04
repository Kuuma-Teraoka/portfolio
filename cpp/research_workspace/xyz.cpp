
/*
　y->xも、xyz_sumもすべて。
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
    matrix ts = coupled_rossler::time_series
    (
        truth_links, 
        dump+learn+pred, 
        rand_dir + "/" + str(r) + ".bin", 
        h_value, 
        c, 
        tau
    ) ;
    truth_links.write(dist_dir + "/truth_links.fmat") ;
    ts.write(dist_dir + "/ts.fmat") ; 



    matrix sum_sum(node, node, 0.0), sum_dia(node, node, 0.0), sum_max(node, node, 0.0), sum_min(node, node, 0.0), sum_low(node, node, 0.0), sum_sud(node, node, 0.0), sum_top(node, node, 0.0) ;
    std::vector<matrix> sum(3 * 3, matrix(node, node, 0.0)) ;
    std::vector<matrix> sum_t(3 * 3, matrix(node, node, 0.0)) ; 

    for(int id = 0 ; id < ensemble ; id++)
    {
        echo_state_network model
        ({
            .Nu = node * 3,
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

        inferred_links = infer_links::bayesian(stcd.block(3, 3, matrix::sum), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_sum_" + str(id) + ".fmat") ;
        sum_sum += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(3, 3, matrix::dia), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_dia_" + str(id) + ".fmat") ; 
        sum_dia += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(3, 3, matrix::max), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_max_" + str(id) + ".fmat") ; 
        sum_max += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(3, 3, matrix::min), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_min_" + str(id) + ".fmat") ;
        sum_min += inferred_links ;
 
        inferred_links = infer_links::bayesian(stcd.block(3, 3, matrix::low), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_low_" + str(id) + ".fmat") ;
        sum_low += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(3, 3, matrix::sud), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_sud_" + str(id) + ".fmat") ;
        sum_sud += inferred_links ;

        inferred_links = infer_links::bayesian(stcd.block(3, 3, matrix::top), edge) ;
        infer_links::confusion_matrix(truth_links, inferred_links).write
            (dist_dir + "/confusion_mat_top_" + str(id) + ".fmat") ;
        sum_top += inferred_links ;

        for(int i = 0 ; i < 3 ; i++)
            for(int j = 0 ; j < 3 ; j++)
            {
                inferred_links = infer_links::bayesian
                    (stcd({i,-1,3}, {j,-1,3}), edge) ;
                infer_links::confusion_matrix(truth_links, inferred_links).write
                    (dist_dir + "/confusion_mat_" + str(i) + str(j) + "_" + str(id) + ".fmat") ;
                sum[i*3 + j] += inferred_links ;
            }

        for(int i = 0 ; i < 3 ; i++)
            for(int j = 0 ; j < 3 ; j++)
            {
                inferred_links = infer_links::bayesian
                    (stcd.T()({i,-1,3}, {j,-1,3}), edge) ;
                infer_links::confusion_matrix(truth_links, inferred_links).write
                    (dist_dir + "/confusion_mat_t" + str(i) + str(j) + "_" + str(id) + ".fmat") ;
                sum_t[i*3 + j] += inferred_links ;
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

            for(int i = 0 ; i < 3 ; i++)
                for(int j = 0 ; j < 3 ; j++)
                {
                    sum[i*3 + j].write(dist_dir + "/sum_" + str(i) + str(j) + "_" + str(id + 1) + ".fmat") ;
                    infer_links::confusion_matrix(truth_links, sum[i*3 + j].above((id + 1) / 2)).write(dist_dir + "/confusion_mat_total_" + str(i) + str(j) + "_" + str(id + 1) + ".fmat") ;
                }
            for(int i = 0 ; i < 3 ; i++)
                for(int j = 0 ; j < 3 ; j++)
                {
                    sum_t[i*3 + j].write(dist_dir + "/sum_t" + str(i) + str(j) + "_" + str(id + 1) + ".fmat") ;
                    infer_links::confusion_matrix(truth_links, sum_t[i*3 + j].above((id + 1) / 2)).write(dist_dir + "/confusion_mat_total_t" + str(i) + str(j) + "_" + str(id + 1) + ".fmat") ;
                }
        }

        
    } 
    
}

