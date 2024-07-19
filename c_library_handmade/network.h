#ifndef _network_h
#define _network_h




struct vertex
{
    unsigned int id ;
    unsigned int level ;

    struct vertex *next ;
} ;

// 幅優先探索のリスト。
struct job
{
    unsigned int id ;
    unsigned int cost ;

    struct job *next ;
} ;

struct adjacency_list
{
    unsigned int id ;
    struct vertex *head ;

    struct adjacency_list *next ;
} ;

struct edges
{
    unsigned int id ;
    unsigned int id2 ;

    struct edges *next ;
} ;


void make_edges_list(struct edges **, unsigned int, unsigned int) ;
struct vertex *edges_list_to_vertex_list(struct edges *) ;
struct adjacency_list *edges_list_to_adjacency_list(struct edges *) ;
void make_adjacency_list(struct adjacency_list **, unsigned int, unsigned int) ;
struct vertex *adjacency_list_to_vertex_list(struct adjacency_list *) ;

void print_adjacency_list(struct adjacency_list *) ;
void print_edges_list(struct edges *) ;
void print_degree_distribution_divided(struct vertex *) ;

double clustering_coefficient(struct adjacency_list *) ;
int is_adjacent(struct adjacency_list *, unsigned int, unsigned int) ;

double average_shortest_path_length(struct adjacency_list *) ;
unsigned int shortest_path_length(struct adjacency_list *, unsigned int, unsigned int) ;
struct vertex *explore_for_shortest_path_length(struct adjacency_list *, unsigned int) ;

double average_degree(struct vertex *) ;

struct vertex *degree_distribution(struct vertex *) ;
void make_degree_distribution_list(struct vertex **, unsigned int) ;

double degree_correlation(struct edges *) ;

struct edges *lattice(unsigned int, unsigned int) ;
void ws_model(double, struct edges *) ;
struct edges *ba_model(unsigned int, unsigned int, unsigned int) ;
void selectVertexByDegree(unsigned int, unsigned int *, unsigned int, unsigned int *) ;

struct vertex *serch_id_vertex(struct vertex *, unsigned int) ;
struct adjacency_list *serch_id_adjacency_list(struct adjacency_list *, unsigned int) ;
unsigned int sum_level_vertex(struct vertex *) ;//?

void append_to_tail_vertex(struct vertex **, struct vertex *) ;
struct vertex *get_tail_vertex(struct vertex *) ;
unsigned int length_vertex(struct vertex *) ;
void free_list_vertex(struct vertex **) ;
void remove_from_list_vertex(struct vertex **, struct vertex *) ;


// vertex->jobにしてコピペしただけ。

void append_to_tail_job(struct job **, struct job *) ;
struct job *get_tail_job(struct job *) ;
unsigned int length_job(struct job *) ;
void free_list_job(struct job **) ;
void remove_from_list_job(struct job **, struct job *) ;

void append_to_tail_adjacency_list(struct adjacency_list **, struct adjacency_list *) ;
struct adjacency_list *get_tail_adjacency_list(struct adjacency_list *) ;
unsigned int length_adjacency_list(struct adjacency_list *) ;
void free_list_adjacency_list(struct adjacency_list **) ;
void remove_from_list_adjacency_list(struct adjacency_list **, struct adjacency_list *) ;

void append_to_tail_edges(struct edges **, struct edges *) ;
struct edges *get_tail_edges(struct edges *) ;
unsigned int length_edges(struct edges *) ;
void free_list_edges(struct edges **) ;
void remove_from_list_edges(struct edges **, struct edges *) ;

#endif
