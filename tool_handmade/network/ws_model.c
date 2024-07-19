/*
./ws_model -n 1000 -k 10 -p 0.05
　というようにオプションで指定すると、
1 0
1 2
2 3
　というように辺リスト形式でwsモデルのネットワークを生成し標準出力するプログラム。
-n:頂点数
-k:円環格子グラフの各頂点の次数
-p:rewiring確率
--help:処理終了して、ws_model -n 20 -k 4 -p 0.01と出力する。



　課題に関して。small networkの論文の再現をする際、このプログラムは確率をオプションで受け取ってネットワークを出力するので、
ws_model -n 20 -k 4 -p 0.01 > clustering_coefficient
　でいくつかのpの値でクラスター係数を求めて、
p, clusterring_coefficient
0.001, 1
0.01, 0.7
0.1, 0.3
1, 0.1
　みたいなデータが必要になるわけで、これはシェルのforを使うしかない。シェル変数に0.001 0.01 0.1という複数の文字列を代入し、for分でtxtにechoして、あとはtxtとgnpからgnuplotでepsにする。

for i in `outputExp`; do echo -n "$i "; divide `ws_model -n 1000 -k 10 -p $i | calucurate_network --clustering-coefficient` `ws_model -n 1000 -k 10 -p 0 | calucurate_network --clustering-coefficient`; done

for i in `outputExp`; do echo -n "$i "; divide `ws_model -n 1000 -k 10 -p $i | calucurate_network --average-shortest-path-length` `ws_model -n 1000 -k 10 -p 0 | calucurate_network --average-shortest-path-length`; done
*/

#include <stdio.h>
#include <stdlib.h>

#include <network.h>
#include <bin.h>


int main(int argc, char **argv)
{
    // counter
    unsigned int i ;

    // option
    unsigned int n, k ;
    double p ;

    // これしないとオプション入れ忘れたときに10000とか入ってたら10000で生成しちゃう。n=0でなにも出力しない、k<2でなにも出力しない。p=0でつなぎかえない。
    n = 0 ;
    k = 0 ;
    p = 0 ;

    // 適当な文字列をオプションに入れても、いくつかのうち定義されたものだけ取り出して読み込むので関係ない。++iのところはi+1でもいいのだが、-n 23の23が引っかかることはないので飛ばしているだけ。++iで+1してからその値を代入している。
    for(i = 0 ; i < argc ; i++)
    {
        if(str_cmp(argv[i], "-n", length_string(argv[i]) + 1) && i + 1 < argc) n = str_to_ui(argv[++i]) ;
        else if(str_cmp(argv[i], "-k", length_string(argv[i]) + 1) && i + 1 < argc) k = str_to_ui(argv[++i]) ;
        else if(str_cmp(argv[i], "-p", length_string(argv[i]) + 1) && i + 1 < argc) p = str_to_d(argv[++i]) ; 

        else if(str_cmp(argv[i], "--help", length_string(argv[i]) + 1))
        {
            printf("use like \"ws_model -n 20 -k 4 -p 0.01\"\n") ;
            return 0 ;
        }
    }

    struct edges *head ;

    head = lattice(n, k) ;
    ws_model(p, head) ;
    print_edges_list(head) ;
    free_list_edges(&head) ;

    return 0 ;
}
