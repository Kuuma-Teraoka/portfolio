
#include "str.hpp"
#include "file.hpp"
#include "matrix.hpp"
#include "network.hpp"
#include "coupled_lorenz.hpp"
#include "echo_state_network.hpp"
#include "infer_links.hpp"

int main()
{
    const str simulation_dir = "dist" ;
    const str rand_dir = file::environ("RINDFILE_PATH") ;
    file::makedir(simulation_dir) ;
    

    
    int node = 2 ;
    int edge = 1 ;
    double h_value = 0.06 ;
    double c = 0.3 ;
    double tau = 0.02 ;
    int dump = 0 ;
    int time = 10 ;
    int pred = 1 ; 
    double beta = 0.0001 ;
    echo_state_network model
    (
        node * 3, // Nu, input demention
        0.1, // w_in: [-0.1, 0.1], (Nu, Nx)
        8, // Nx, neuron mount
        network::random(8, 24, file(rand_dir + "/1.bin")), // neuron links, w_x
        0.9, // spectral radius of w_x
        file(rand_dir + "/1.bin")
    ) ;


    /*matrix akl = network::random(node, edge, file(rand_dir + "/1.bin")) ; 
    matrix ts = coupled_lorenz::time_series
    (
        akl, 
        dump+time+pred, 
        file(rand_dir + "/1.bin"), 
        h_value, 
        c, 
        tau
    ) ; 
 
    akl.write("akl.cmat") ; 
    ts.write("ts.cmat") ;
    model.w_in.write("w_in.cmat") ;
    model.w_x.to_matrix().write("w_x.cmat") ;
    model.x.write("x0.cmat") ; 
    return 0 ;*/

    matrix akl("akl.cmat"), ts("ts.cmat") ;  

    matrix y = model.train(ts(slice(dump,dump+time)),ts(slice(dump+1,dump+1+time)),beta) ;
    model.w_out.write("w_out.cmat") ;
    model.x.write("x5.cmat") ;
    return 0 ;
    //matrix y_pred = model.feedback() ;
    matrix stcd = infer_links::stcd(model, ts(slice(dump,dump+time)), tau) ;
    matrix bayesian_akl = infer_links::bayesian(stcd(slice(0,-1,3),slice(1,-1,3)),edge) ;
    matrix confusion_matrix = infer_links::confusion_matrix(akl, bayesian_akl) ;
    

    akl.write(simulation_dir + "/akl.fmat") ;
    ts.write(simulation_dir + "/ts.fmat") ;
    model.write(simulation_dir + "/model.esn") ;
    y.write(simulation_dir + "/y.fmat") ;
    stcd.write(simulation_dir + "/stcd.fmat") ;
    confusion_matrix.write(simulation_dir + "/confusion_matrix.fmat") ;

    akl.write(simulation_dir + "/akl.cmat") ;
    ts.write(simulation_dir + "/ts.cmat") ;
    model.w_in.write(simulation_dir + "/w_in.cmat") ;
    model.w_x.to_matrix().write(simulation_dir + "/w_x.cmat") ;
    model.w_out.write(simulation_dir + "/w_out.cmat") ;

    return 0 ;
}


