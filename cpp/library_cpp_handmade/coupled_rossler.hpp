
#ifndef _coupled_rossler_hpp
#define _coupled_rossler_hpp

#include "file.hpp"
#include "matrix.hpp"
#include "str.hpp"
#include "progress.hpp"
#include "randgen.hpp"

namespace coupled_rossler
{
    // 状態を与えると微分方程式を解いて変化量を返す関数
    matrix differential_equations(const matrix &vec, const matrix &truth_links, const matrix &h_vec, const randgen &rangen, double c)
    { 
        matrix result(vec) ; 
        for(int i = 0 ; i < vec.shape[0] / 3 ; i++)
        {
            double x = vec(3 * i + 0, 0) ;
            double y = vec(3 * i + 1, 0) ;
            double z = vec(3 * i + 2, 0) ;
            double x_sum = 0, y_sum = 0, z_sum = 0 ;
            for(int j = 0 ; j < truth_links.shape[0] ; j++)
            {
                x_sum += truth_links(i, j) * (vec(3 * j + 0, 0) - x) ;
                y_sum += truth_links(i, j) * (vec(3 * j + 1, 0) - y) ;
                z_sum += truth_links(i, j) * (vec(3 * j + 2, 0) - z) ;
            }
            result(3 * i + 0, 0) = -1 * h_vec(i, 0) * y - z + c * x_sum ;
            result(3 * i + 1, 0) = h_vec(i, 0) * x + 0.2 * y + c * y_sum ;
            result(3 * i + 2, 0) = 0.2 + z * (x - 9.0) + c * z_sum ;
        }
        return result ;
    }

    
    // 状態を与えるとeauations()で変化量を求めて、ルンゲクッタ法により次の時刻の状態を返す関数。
    matrix runge_kutta(const matrix &vec, const matrix &truth_links, const matrix &h_vec, const randgen &rangen, double c, double tau)
    { 
        matrix k1 = tau * differential_equations(vec, truth_links, h_vec, rangen, c) ;
        matrix k2 = tau * differential_equations(vec + 0.5*k1, truth_links, h_vec, rangen, c) ;
        matrix k3 = tau * differential_equations(vec + 0.5*k2, truth_links, h_vec, rangen, c) ;
        matrix k4 = tau * differential_equations(vec + k3, truth_links, h_vec, rangen, c) ; 
        matrix result = vec + 1.0 / 6 * (k1 + 2 * k2 + 2 * k3 + k4) ;

        return result ;
    }
 
    matrix time_series
    (
        const matrix &truth_links, 
        unsigned int time=60000,
        const str &rand_file_name=file::environ("RINDFILE_PATH") + str("/1.bin"),
        std::pair<double, double> h_value = {0.8, 1.2},
        double c=0.3,
        double tau=0.02
    )
    {
        matrix result(truth_links.shape[0]*3, time) ;
        result(0) = matrix::random(result.shape[0], 1, rand_file_name) ; 
        matrix h_vec = matrix::random(truth_links.shape[0], 1, rand_file_name, h_value.first, h_value.second) ;
        matrix tmp = result(0) ;
        
        std::cout << "coupled_rossler(): calculate..." << std::flush ;
        progress bar ;
        for(int i = 0 ; i < (time-1)*20 ; i++)
        {
            tmp = runge_kutta(tmp, truth_links, h_vec, rand_file_name, c, tau / 20) ; 
            if((i+1) % 20 == 0)
                result((i+1) / 20) = tmp ; 
        }
        std::cout << "complete in " << bar.elapsed() << " [sec]" << "\n" ;
 
        return result ;
    }




   
    


}

#endif

