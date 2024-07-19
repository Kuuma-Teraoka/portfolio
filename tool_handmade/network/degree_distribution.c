#include <stdio.h>
#include <stdlib.h>

#include <bin.h>
#include <network.h>

int main(int argc, char **argv)
{
    // fgets用
    char *std_line, **strv ;
    unsigned int strc ;

    // カウンタ
    unsigned int i ;


    struct edges *head_edges = NULL ;

    std_line = (char *)malloc(sizeof(char) * 128) ;

    while(fgets(std_line, 128, stdin) != NULL)
    {
        str_split(length_string(std_line), std_line, &strc, &strv) ;



        // メイン処理1。リストに入れてく。制約で文字列2つとする。コメント分スキップのためifにしたので文字列2つなくてもちゃんとスキップするようにした。
        if(strc >= 2 && strv[0][0] != '#')
            make_edges_list(&head_edges, str_to_ui(strv[0]), str_to_ui(strv[1])) ;



        for(i = 0 ; i < strc ; i++)
            free(strv[i]) ;
        free(strv) ;
    }

    free(std_line) ;





    // メイン処理2。
    struct vertex *head_vertex, *head_degree_distribution_list ;
    head_vertex = edges_list_to_vertex_list(head_edges) ;
    head_degree_distribution_list = degree_distribution(head_vertex) ;
    print_degree_distribution_divided(head_degree_distribution_list) ;
    free_list_edges(&head_edges) ;
    free_list_vertex(&head_vertex) ;
    free_list_vertex(&head_degree_distribution_list) ;




    return 0 ;
}

