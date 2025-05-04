
#include "echo_state_network.hpp"
#include "network.hpp"
#include "matrix.hpp"
#include "file.hpp"

int main()
{
    // write()。書き込み前のw_out, 書き込んで読み込んだw_out, .esnファイルの書き込み読み込みのw_outで一致。
    echo_state_network model(3, 0.1, 5,  network::random(5, 10)) ;
    model.train(matrix::random(3, 10), matrix::random(3, 10)) ;
    std::cout 
        << "model.w_out:\n"
        << model.w_out
        << "model.w_x.to_matrix():\n"
        << model.w_x.to_matrix() ;

    file::makedir("dist") ;
    model.write("dist/model.esn") ;
    model.w_in.write("dist/w_in.fmat") ;
    model.w_x.write("dist/w_x.csr") ;
    model.w_out.write("dist/w_out.fmat") ;
    model.x.write("dist/x.fmat") ;  
    std::cout 
        << "echo_state_network(\"model.esn\").w_out:\n"
        << echo_state_network("dist/model.esn").w_out
        << "echo_state_network(\"model.esn\").w_x.to_matrix():\n"
        << echo_state_network("dist/model.esn").w_x.to_matrix()
        << "matrix(\"w_out.fmat\"):\n"
        << matrix("dist/w_out.fmat")
        << "matrix::csr(\"dist/w_x.csr\").to_matrix():\n"
        << matrix::csr("dist/w_x.csr").to_matrix() ;
}

