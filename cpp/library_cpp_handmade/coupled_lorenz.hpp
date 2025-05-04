
#ifndef _coupled_lorenz_hpp
#define _coupled_lorenz_hpp

#include "file.hpp"
#include "matrix.hpp"
#include "str.hpp"
#include "progress.hpp"

/*
https://doi.org/10.3182/20090622-3-UK-3004.00014
https://doi.org/10.34133/2022/9870149
　にcoupled lorenzとあるので、それをもとに命名。

　0.001で20倍で、60*90000*20で01:04。-O3ありだと00:09。
*/

namespace coupled_lorenz
{
    matrix runge_kutta(const matrix &, const matrix &, const matrix &, double, double) ;
    matrix differential_equations(const matrix &, const matrix &, const matrix &, double) ;


    // パラメータを与えると結合ローレンツの時系列を生成し、matrixで返す関数。
    matrix time_series
    (
        const matrix &truth_links, 
        unsigned int time=60000,
        const file &rand_file=file(file::environ("RINDFILE_PATH") + str("/1.bin")),
        double h_value=0.06, 
        double c=0.3,
        double tau=0.02
    )
    {
        matrix result(truth_links.shape[0]*3, time) ;
        result(0) = matrix::random(result.shape[0], 1, rand_file) ; 
        matrix h_vec = matrix::random(truth_links.shape[0], 1, rand_file, -1 * h_value, h_value) ;
        matrix tmp = result(0) ;
        
        std::cout << "coupled_lorenz(): calculate..." << std::flush ;
        progress bar ;
        for(int i = 0 ; i < (time-1)*20 ; i++)
        {
            tmp = runge_kutta(tmp, truth_links, h_vec, c, tau / 20) ; 
            if((i+1) % 20 == 0)
                result((i+1) / 20) = tmp ; 
        }
        std::cout << "complete in " << bar.elapsed() << " [sec]" << "\n" ;
 
        return result ;
    }




    
    // 状態を与えるとeauations()で変化量を求めて、ルンゲクッタ法により次の時刻の状態を返す関数。
    matrix runge_kutta(const matrix &vec, const matrix &truth_links, const matrix &h_vec, double c, double tau)
    { 
        matrix k1 = tau * differential_equations(vec, truth_links, h_vec, c) ;
        matrix k2 = tau * differential_equations(vec + 0.5*k1, truth_links, h_vec, c) ;
        matrix k3 = tau * differential_equations(vec + 0.5*k2, truth_links, h_vec, c) ;
        matrix k4 = tau * differential_equations(vec + k3, truth_links, h_vec, c) ; 
        matrix result = vec + 1.0 / 6 * (k1 + 2 * k2 + 2 * k3 + k4) ;

        return result ;
    }

    // 状態を与えると微分方程式を解いて変化量を返す関数
    matrix differential_equations(const matrix &vec, const matrix &truth_links, const matrix &h_vec, double c)
    { 
        matrix result = vec ;
        double sigma = 10 ;
        double rho = 28 ;
        double beta = 8.0 / 3 ;
        for(int i = 0 ; i < vec.shape[0] / 3 ; i++)
        {
            double x = vec(3 * i + 0, 0) ;
            double y = vec(3 * i + 1, 0) ;
            double z = vec(3 * i + 2, 0) ;
            double sum = 0 ;
            for(int j = 0 ; j < truth_links.shape[0] ; j++)
                sum += truth_links(i, j) * (vec(3 * j + 1, 0) - y) ;
            result(3 * i + 0, 0) = -1 * sigma * (x - y + c * sum) ;
            result(3 * i + 1, 0) = rho * (1 + h_vec(i, 0)) * x - y - x * z ;
            result(3 * i + 2, 0) = -1 * beta * z + x * y ;
        }
        return result ;
    }


}

#endif

