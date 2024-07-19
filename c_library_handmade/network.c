#include <stdio.h>
#include <stdlib.h>

#include <bin.h>
#include <mymath.h>
#include <random.h>
#include <network.h>



// 整数2つを受け取って、辺リストに加える関数。
void make_edges_list(struct edges **head, unsigned int a, unsigned int b)
{
    struct edges *new_edges ;

    new_edges= (struct edges *)malloc(sizeof(struct edges)) ;
    new_edges->id = a ;
    new_edges->id2 = b ;
    new_edges->next = NULL ;

    append_to_tail_edges(head, new_edges) ;
}

// とりあえずedges->adjacency->vertexにしているが、ちゃんと作ったほうが効率よくなる。
struct vertex *edges_list_to_vertex_list(struct edges *head)
{
    struct adjacency_list *head_adjacency_list ;
    struct vertex *head_vertex ;

    head_adjacency_list = edges_list_to_adjacency_list(head) ;
    head_vertex = adjacency_list_to_vertex_list(head_adjacency_list) ; 

    free_list_adjacency_list(&head_adjacency_list) ;
    
    return head_vertex ;
}



// vertexのリストをadjacency_listのリストに変換する関数。可逆。
struct adjacency_list *edges_list_to_adjacency_list(struct edges *head)
{
    struct adjacency_list *head_adjacency_list = NULL ;

    for( ; head != NULL ; head = head->next)
    {
        make_adjacency_list(&head_adjacency_list, head->id, head->id2) ;
        make_adjacency_list(&head_adjacency_list, head->id2, head->id) ;
    }

    return head_adjacency_list ;
}



// 整数二つを受け取って、adjacency_listのリストに入れていく関数。3と4を受け取ったら、3のリストに4を追加するだけなので、逆の4と3もやらなきゃいけない。
void make_adjacency_list(struct adjacency_list **head, unsigned int a, unsigned int b)
{
    struct vertex *new_vertex ;
    struct adjacency_list *current_adjacency_list ;

    if((current_adjacency_list = serch_id_adjacency_list(*head, a)) != NULL)
    {
        if(!(serch_id_vertex(current_adjacency_list->head, b) != NULL))
        {
            new_vertex = (struct vertex *)malloc(sizeof(struct vertex)) ;
            new_vertex->id = b ;
            new_vertex->next = NULL ;
    
            append_to_tail_vertex(&(current_adjacency_list->head), new_vertex) ;
        }
        // どっちも見つかった場合、つまり辺がすでにリストにある場合。基本的にないが一応。被りなしの制約は必要ない。
        else
        {
            // とりあえず何もしない
            //fputs("this edge have already been inputed.\n", stdout) ;
        }
    }
    // 2次元目のリストも空確定。
    else
    {
        current_adjacency_list = (struct adjacency_list *)malloc(sizeof(struct adjacency_list)) ;
        current_adjacency_list->id = a ;
        current_adjacency_list->head = (struct vertex *)malloc(sizeof(struct vertex)) ;
        current_adjacency_list->next = NULL ;

        current_adjacency_list->head->id = b ;
        current_adjacency_list->head->next = NULL ;
        
        append_to_tail_adjacency_list(head, current_adjacency_list) ;
    }
}



//　adjacency_listのリストからvertexのリストを生成する関数。不可逆。
struct vertex *adjacency_list_to_vertex_list(struct adjacency_list *head)
{
    struct vertex *new_vertex, *head_vertex = NULL ;

    for( ; head != NULL ; head = head->next)
    {
        new_vertex = (struct vertex *)malloc(sizeof(struct vertex)) ;
        new_vertex->id = head->id ;
        new_vertex->level = length_vertex(head->head) ;

        append_to_tail_vertex(&head_vertex, new_vertex) ;
    }

    return head_vertex ;
}



/*
　adjacency_listのリストを受け取って

1:2,4,
2:1,
3:
4:1,
　と出力する関数。挙動としては、構造体のポインタを受け取って、そのnextを辿るだけなので、リストの途中の構造体ポインタを与えればそれ以降のリストを表示してくれる。
　一瞬、引数のポインタをカウンタのようにして書き換えてしまったら、mainのheadが末尾になってしまうかもと思ったのだが、ポインタではないので値は書き換えられない。つまり、headの位置をhead_adjacency_listという変数で受け取っただけで、その変数の値を変えてもmainのheadは変わらず、自由に使うことができる。headの位置のポインタを受け取った場合は、その変数をaとすれば*aがheadの位置であり、これを変えるとmainのも変わってしまう。
*/
void print_adjacency_list(struct adjacency_list *head_adjacency_list)
{
    struct vertex *current ;

    for(; head_adjacency_list != NULL ; head_adjacency_list = head_adjacency_list->next)
    {
        printf("\n%d:", head_adjacency_list->id) ;
        for(current = head_adjacency_list->head ; current != NULL ; current = current->next)
            printf("%d,", current->id) ;
    }
    printf("\n") ;
}



void print_edges_list(struct edges *head)
{
    for( ; head != NULL ; head = head->next)
        printf("%u %u\n", head->id, head->id2) ;
}


// 次数分布のリストを受け取って、それを頂点数で割って正規化した次数分布を標準出力する関数。
void print_degree_distribution_divided(struct vertex *head)
{
    unsigned int sum = sum_level_vertex(head) ;

    for( ; head != NULL ; head=head->next)
        printf("%d %f\n", head->id, (double)head->level / sum) ;
}



double clustering_coefficient(struct adjacency_list *head)
{
    double sum = 0 ;
    struct vertex *i, *j ;
    struct adjacency_list *current_adjacency_list ;
    unsigned int cluster_counter ;

    // 次数が1以下の頂点はj!=NULLで抜けて、cluster_counter=0のままsumは変化しないのでifは必要ないように思えたが、combinationの値が0になりnanになってしまうので注意。
    for(current_adjacency_list = head ; current_adjacency_list != NULL; current_adjacency_list = current_adjacency_list->next)
        if(length_vertex(current_adjacency_list->head) > 1)
        {
            cluster_counter = 0 ;
            for(i = current_adjacency_list->head ; i != NULL ; i = i->next)
                for(j = i->next ; j != NULL ; j = j->next)
                    if(is_adjacent(head, i->id, j->id))
                        cluster_counter++ ;
            sum += (double)cluster_counter / combination(length_vertex(current_adjacency_list->head), 2) ;
        }

    return (sum / length_adjacency_list(head)) ;
}



// adjacency_listと頂点を表すふたつの整数を受け取って、繋がっていれば1、繋がっていなければ0を返す関数。1と3が繋がっていれば1に3、3に1が追加されているはずなので、とりあえず1に3があるか調べている。
int is_adjacent(struct adjacency_list *head, unsigned int a, unsigned int b)
{
    if(serch_id_adjacency_list(head, a) == NULL) return 0 ;
    else if(serch_id_vertex(serch_id_adjacency_list(head, a)->head, b) == NULL) return 0 ;
    else return 1 ;
}



/*
　adjacency_listを受けとって、平均最短パス長を返す関数。0 1, 2 0, 2 1,...とnC2個を足すわけだが、幅探索ではn個の配列が返ってくる。最短パス長の行列を間耐えて、
0 & 1 & 3
1 & 0 & 2
3 & 2 & 0
　という形で、nC2は3つの値からなる三角形。n個の配列をn回足すと三角形2つとななめの合計になる。だが、3を探索したときに3には0が入っているので足しても問題ない。なのでこれは三角形2つの合計になり、2で割れば目的のnC2の和が求められることになる。
*/
double average_shortest_path_length(struct adjacency_list *head)
{
    struct adjacency_list *i ;
    struct vertex *head_vertex, *j ;
    unsigned int isolated_point_counter, sum ;

    isolated_point_counter = 0 ;
    sum = 0 ;

    for(i = head ; i != NULL ; i = i->next)
    {
        head_vertex = explore_for_shortest_path_length(head, i->id) ;
        for(j = head_vertex ; j != NULL ; j = j->next)
        {
            if(j->level == 0 - 1)
                isolated_point_counter++ ;
            else
                sum += j->level ;
        }
        free_list_vertex(&head_vertex) ;
    }

    return ((double)sum / ((2 * combination(length_adjacency_list(head), 2)) - isolated_point_counter)) ;
}



// こっちは初期型。最短パス長を調べる際に片方の頂点から幅優先探索をしていき、頂点のリストを用意して、最短距離を更新していくのだが、このときにa, bだけでなく、aと、aと隣接しているすべての頂点の最短パス長が同時に求まる。a, bの最短パス長を知りたいだけなら切り捨てていいが、すべての頂点の組の最短パス長を調べるのだから切り捨ててしまえば無駄になる。また、頂点数nとすると、最短パス長の幅優先探索を、切り捨てた場合はnC2、切り捨てない場合は、n回行うことになる。簡単にするとn^2とnの違いなので、頂点数10程度なら100で済むが、頂点数1,000となると、1,000,000となり計算量が無駄に増えることになる。
/*
double culcurateAverageShortestPathLength(struct vertex *head)
{
    double sum = 0 ;
    struct vertex *i, *j ;

            
    for(i = head ; i != NULL ; i = i->next) for(j = i->next ; j != NULL ; j = j->next) sum += culcurateShortestPathLength(head, i->id, j->id) ;

    return (sum / combination(length_vertex(head), 2)) ;
}
*/



// adjacency_listのリストと二つの頂点を受け取って、最短パス長を返す関数。
unsigned int shortest_path_length(struct adjacency_list *head, unsigned int a, unsigned int b)
{
    struct vertex *head_vertex ;
    unsigned int answer ;

    head_vertex = explore_for_shortest_path_length(head, a) ;
    answer = serch_id_vertex(head_vertex, b)->level ;
    free_list_vertex(&head_vertex) ;

    return answer ;
}



// adjacency_listのリストとひとつの頂点を受け取って、その頂点から幅優先探索を行い、最短パス長をいれた頂点のリストを返す関数。mallocしたものを返すので、呼び出し元でfreeする必要がある。
struct vertex *explore_for_shortest_path_length(struct adjacency_list *head, unsigned int a)
{
    struct vertex *current_vertex, *new_vertex ;
    struct job *current_job, *new_job ;
    struct adjacency_list *current_adjacency_list ; 

    // 幅探索用のjobのリスト
    struct job *head_job = NULL ;
    // vertexのリストを作り、次数用に定義したlevel変数を使って、距離の最小値を記録していく。
    struct vertex *head_vertex = NULL ;

    // 初期化。
    for(current_adjacency_list = head ; current_adjacency_list != NULL ; current_adjacency_list = current_adjacency_list->next)
    {
        new_vertex = (struct vertex *)malloc(sizeof(struct vertex)) ;
        new_vertex->id = current_adjacency_list->id ;
        // 探索用の変数は0で初期化しておく。
        new_vertex->level = 0 - 1 ;
        new_vertex->next = NULL ;

        append_to_tail_vertex(&head_vertex, new_vertex) ;
    }

    new_job = (struct job *)malloc(sizeof(struct job)) ;
    new_job->id = a ;
    new_job->cost = 0 ;
    new_job->next = NULL ;

    // head_job = new_jobでいいのだが、下と似た感じにしたかった。
    append_to_tail_job(&head_job, new_job) ;

    /*
    job取り出し, 周囲のjobを確認上書きしながら追加, ifNULLbreak, job取り出し, ...
    job取り出し, 最低値確認して上書きなら周囲のjob追加, ifNULLbreak, job取り出し, ...
    　のふたつを思いついた。後者のほうが簡単だと思ったのだが、キューに追加してから最小値確認するなら、追加する前に確認してしまったほうが合理的なので前者のほうがいいだろう。確認のついでに上書きもしてしまったほうがいいだろう。キューに入るjobは、確認上書きと周囲の追加、ではなく、周囲を確認上書きしながら追加というものになる。一応後者のものもコメントアウトで残しておく。
    */

    // 探索時に周囲のjobを確認上書きしながら追加に変えたことで、jobの内容にいまのjobの上書きが含まれなくなったので、一つ目のjobは上書きしておかなければならない。
    serch_id_vertex(head_vertex, new_job->id)->level = 0 ;
    while(head_job != NULL)
    {
        current_job = head_job ;
        remove_from_list_job(&head_job, current_job) ;

        // 周囲のjobを確認上書きしながら追加
        for(current_vertex = serch_id_adjacency_list(head, current_job->id)->head ; current_vertex != NULL ; current_vertex = current_vertex->next)
            if(serch_id_vertex(head_vertex, current_vertex->id)->level > current_job->cost + 1)
            {
                serch_id_vertex(head_vertex, current_vertex->id)->level = current_job->cost + 1 ;
                new_job = (struct job *)malloc(sizeof(struct job)) ;
                new_job->id = current_vertex->id ;
                new_job->cost = current_job->cost + 1 ;
                new_job->next = NULL ;

                append_to_tail_job(&head_job, new_job) ;
            }

        free(current_job) ;
    }

    /*
    while(head_job != NULL)
    {
        current_job = head_job ;
        remove_from_list_job(&head_job, current_job) ;
        current_vertex = serchById(head_vertex, current_job->id) ;

        // 最小値確認
        if(current_vertex->level > current_job->cost)
        {
            // 上書き
            current_vertex->level = current_job->cost ;
            // 周囲のjob追加
            for(current_vertex = serchById(head, current_job->id)->head ; current_vertex != NULL ; current_vertex = current_vertex->next)
            {
                new_job = (struct job *)malloc(sizeof(struct job)) ;
                new_job->id = current_vertex->id ;
                new_job->cost = current_job->cost + 1 ;
                new_job->next = NULL ;

                append_to_tail_job(&head_job, new_job) ;
            }
        }

        free(current_job) ;
    }
    */

    return (head_vertex) ;
}


// vertexのリストを受け取って、平均次数を求める関数。
double average_degree(struct vertex *head)
{
    return (double)sum_level_vertex(head) / length_vertex(head) ; 
}



// vertexのリストを受け取って、次数分布を出力する関数。リスト形式で返す。次数分布用の構造体リストと関数を定義するのがめんどくさいので、vertexと同じ構造体をつかう。少しややこしいが、idがx座標つまり次数でlevelがy座標つまりその次数の頂点の数。次数分布は0 0, 1 0, 2 3, 3 0というような形式にするべきなのだが、ここで返すリストはソートされていないので注意。
struct vertex *degree_distribution(struct vertex *head)
{
    struct vertex *counter, *head_of_target = NULL ;

    for(counter = head ; counter != NULL ; counter = counter->next) make_degree_distribution_list(&head_of_target, counter->level) ;

    return head_of_target ;
}



// 頂点リストとidを与えて、見つかればlevelを1上げて、見つからなければlevel1で新規ノード追加する。
void make_degree_distribution_list(struct vertex **head, unsigned int id)
{
    struct vertex *new_vertex ;

    if((new_vertex = serch_id_vertex(*head, id)) != NULL)
    {
        new_vertex->level++ ;
    }
    else
    {
        new_vertex = (struct vertex *)malloc(sizeof(struct vertex)) ;
        new_vertex->next = NULL ;
        new_vertex->id = id ;
        new_vertex->level = 1 ;

        append_to_tail_vertex(head, new_vertex) ;
    }
}

// 構造体パラメータにdouble必要。
/*
struct vertex *degree_distribution_divided(struct vertex *head)
{
    struct vertex *i, *answer = degree_distribution(head) ;

    for(i = answer ; i != NULL ; i = i->next) i->level
}
*/


// unsigned int idというパラメータを持つ構造体リストをソートする関数。length_vertexで奇数、たとえば5でも、5/2=2で2回nextして1,2と3,4,5に分ける。偶数例えば4は4/2=2で1,2と3,4に分ける。
void sortListByMerge(unsigned int num, struct vertex **head)
{
    if(num != 1)
    {
        unsigned int i ;
        struct vertex *head2 = *(head) ;

        for(i = 0 ; i < num / 2 ; i++)
            head2 = head2->next ;

        sortListByMerge(num / 2, head) ;
        sortListByMerge((num - num / 2), &head2) ;



    }





}


double degree_correlation(struct edges *head)
{
    struct vertex *head_vertex ;
    unsigned int summation, product, sum_of_squares, num_edges, i, j ;

    head_vertex = edges_list_to_vertex_list(head) ;
    summation = product = sum_of_squares = 0 ;
    num_edges = length_edges(head) ;

    for( ; head != NULL ; head = head->next)
    {
        i = serch_id_vertex(head_vertex, head->id)->level - 1 ;
        j = serch_id_vertex(head_vertex, head->id2)->level - 1 ;

        summation += i + j ;
        product += i * j ;
        sum_of_squares += i * i + j * j ;
    }

    return (((double)product / num_edges - (double)summation / 2 / num_edges * summation / 2 / num_edges) / ((double)sum_of_squares / 2 / num_edges - (double)summation / 2 / num_edges * summation / 2 / num_edges)) ;
}



/*
　円環格子のネットワークを生成する関数。nが頂点数で、kが円環格子グラフの各頂点の次数。制約として、
3<=n<=0-1, unsigned int
2<=k<n && k%2==0, unsigned int
　とする。次数が奇数も許されるのかどうかについては論文を読破しないとわからない。とりあえず偶数でつくる。
*/
struct edges *lattice(unsigned int n, unsigned int k)
{
    // counter
    unsigned int i, j ;

    struct edges *new_edges, *head_edges = NULL ;

    for(i = 0 ; i < n ; i++)
        for(j = 1 ; j <= k / 2 ; j++)
        {
            new_edges = (struct edges *)malloc(sizeof(struct edges)) ;
            new_edges->id = i ;
            // %nしないとn=20で18 20, 19 20, 19 21になってしまう。
            new_edges->id2 = (i + j) % n ;
            new_edges->next = NULL ;

            append_to_tail_edges(&head_edges, new_edges) ;
        }
    
    return head_edges ;
}


// latticeを受け取ってつなぎかえ確率pで繋ぎ変えて、latticeをws_modelに変換する関数。
void ws_model(double p, struct edges *head)
{
    struct adjacency_list *head_adjacency_list = edges_list_to_adjacency_list(head) ;
    unsigned int n = length_adjacency_list(head_adjacency_list) ;
    free_list_adjacency_list(&head_adjacency_list) ;

    for( ; head != NULL ; head = head->next)
    {
        if(genrand64_real1() < p)
        {
            // 同じだったら何回も組み替えることで自己ループなしに。どうせ組み換えるので適当に同じになるように初期化。
            head->id = head->id2 ;
            while(head->id == head->id2)
            {
                head->id = (unsigned int)(randomFrom0untill1() * n) ;
                head->id2 = (unsigned int)(randomFrom0untill1() * n) ;
            }
        }
    }
}

/*
　ba modelのネットワークを生成する関数。
　制約は、
1<=m<=m0<=n<=unsignedIntMax, unsigned int

　基本的な挙動として、まずwsモデル生成アルゴリズムについて、複雑ネットワーク.txtからコピペする。
　完全グラフから初めて一つずつ頂点を追加していくもので、最初の頂点の数と、頂点を追加するときに追加される辺の数がパラメータ。仮に4,3とする。最初に頂点を追加する時に、元からあった4つの頂点から3つを選び辺で結ぶ。次に5つから3つ、6つから3つ、とどんどん大きくしていく。そして選び方だが、ある頂点が選ばれる確率は、その頂点の次数を全ての頂点の次数の和で割った値にする。つまり、次数が一番大きい頂点が選ばれやすいので、次数が莫大な頂点が作られやすいのだろう。
　プログラムの挙動としては単純で、まず、m0=4個の頂点を持つ完全グラフの辺を出力、つまり、
1 0
2 0
2 1
3 0
3 1
3 2
　と出力して、アルゴリズムにしたがって、m=3個の頂点を選んで、
4 0
4 2
4 3
　と追加で出力していき、nまでこれを続ければいい。
*/
struct edges *ba_model(unsigned int m, unsigned int m0, unsigned int n)
{
    // counter
    unsigned int i, j ;

    struct edges *head ;
    head = NULL ;
    
    // 選んだm個の頂点を入れる配列。定義にmを使うのでオプションを読み込んだ後に定義する必要がある。
    unsigned int selectedVertex[m] ; 
    
    // 頂点の次数を含む配列。構造体リストと悩んだが、とりあえずこのプログラムでは不変調配列で十分そう。edges_to_vertexをmake_vertex_listを作って作れば、これもvertex_listで実装できる。
    unsigned int vertexDegree[n] ;
    for(i = 0 ; i < n ; i++)
        vertexDegree[i] = 0 ;

    // 完全グラフの生成。2つのカウンターで1 0, 2 0, 2 1とやっていくだけ。
    for(i = 1 ; i < m0 ; i++)
        for(j = 0 ; j < i ; j++)
            make_edges_list(&head, i, j) ;

    for(i = 0 ; i < m0 ; i++)
        vertexDegree[i] = m0 - 1 ;


    // iはすでにm0で上のforを抜けているので初期化の必要はないが、書いたほうがわかりやすいので一応。追加された頂点と、選ばれたm個の頂点を出力していくだけ。
    for(i = m0 ; i < n ; i++)
    {
        selectVertexByDegree(n, vertexDegree, m, selectedVertex) ;

        for(j = 0 ; j < m ; j++)
        {
            make_edges_list(&head, i, selectedVertex[j]) ;
            vertexDegree[selectedVertex[j]]++ ;
        }
        // selectVertex後にしないと4 1, 4 3, 4 4というように自分が選ばれる可能性がある。
        vertexDegree[i] = m ;
    }

    return head ;
}

// 各頂点の次数が入った配列から確率をつくり、それによって、m個の頂点を選び、もうひとつの引数であるm個の要素の配列に、選んだ頂点を入れる関数。同じ頂点が選ばれないように抽選を繰り返す。
void selectVertexByDegree(unsigned int numIn, unsigned int *vertexDegree, unsigned int numOut, unsigned int *selectedVertex)
{
    unsigned int i ;
    double probablity[numIn] ;
    unsigned int sumDegree ;

    sumDegree = 0 ;
    for(i = 0 ; i < numIn ; i++) sumDegree += vertexDegree[i] ;

    for(i = 0 ; i < numIn ; i++) probablity[i] = (double)vertexDegree[i] / sumDegree ;

    // 抽選, check, 抽選, ...
    do
    {
        for(i = 0 ; i < numOut ; i++) selectedVertex[i] = roulet(numIn, probablity) ;
    } while(checkDifferArray(selectedVertex, numOut));
}















// 検索。双方向、単方向どちらでも機能する。必要パラメータ:next, id。リストからidに一致する構造体を見つけ、そのポインタを返す関数。見つからなければNULLを返す。
struct vertex *serch_id_vertex(struct vertex *head, unsigned int id)
{
    // head==NULLでもforを抜けてNULLを返す。
    for( ; head != NULL ; head = head->next)
        if(id == head->id)
            break ;

    return head ;
}

struct adjacency_list *serch_id_adjacency_list(struct adjacency_list *head, unsigned int id)
{
    for( ; head != NULL ; head = head->next)
        if(id == head->id)
            break ;

    return head ;
}

// levelの和を求める関数。必要パラメータ:next, level。本来の使い方では次数合計を求めることになるが、最短パス長の関数において、最短パス長を保存するのにlevelを使っているので、最短パス長の和を求める関数になる。
unsigned int sum_level_vertex(struct vertex *head)
{
    struct vertex *i ;
    unsigned int sum ;

    sum = 0 ;

    for(i = head ; i != NULL ; i = i->next) sum += i->level ;

    return sum ;
}






// 以下すべて必要パラメータはnextのみ。つまり単方向リスト用関数。

// 単方向リスト用の追加。必要パラメータ:next。違う構造体だとwarningが出るだけでもしかしたらいけるのかと思ったのだが、いけなかったりいけたりと謎のエラーに苦しめられる。おそらく同じnextというパラメータを持っていても、構造体ごとに大きさが違うのが原因か。いけるときといけないときを構造体のバイナリをもとに理解したい。律儀にnew_element->nextにNULLを入れているが作る時にちゃんとしていれば必要ない。
void append_to_tail_vertex(struct vertex **head, struct vertex *new_element)
{
    if(*head == NULL)
    {
        *head = new_element ;
        new_element->next = NULL ;
    }
    else
    {
        get_tail_vertex(*head)->next = new_element ;
        new_element->next = NULL ;
    }
}

// 単方向でtailを取得する関数。必要パラメータ:next。他のところにも書いているが、引数のカウンタ化はポインタでない限り、ただ変数をつくってそこに引数の値を入れているだけなのでmainで変数が書き変わるという心配はない。
struct vertex *get_tail_vertex(struct vertex *head)
{
    for( ; head->next != NULL ; head = head->next) ;
    return head ;
}

// ノード数。双方向、単方向どちらでも機能する。必要パラメータ:next。
unsigned int length_vertex(struct vertex *head)
{
    unsigned int i = 0 ;
    for( ; head != NULL ; head = head->next)
        i++ ;

    return i ;
}

// リストの解放。双方向、単方向どちらでも機能する。必要パラメータ:next。ちゃんとheadをNULLにして返す。freeするとnextが使えないため、変数をもうひとつ用意してnextを保存しておかなければならない。よって、ふたつ変数が必要になり、引数のheadを使っている。ほかのところで引数をカウンタをして使っているのとはちがって、この関数はポインタを引数にしているため書き換えてしまっていることに注意。だが、どうせNULLにして返すので問題ない。
void free_list_vertex(struct vertex **head)
{
    struct vertex *tmp ;

    while(*head != NULL)
    {
        tmp = *head ;
        *head = (*head)->next ;
        free(tmp) ;
    }
}

// リストの抜き出し。単方向。必要パラメータ:next。headのリストからは完全に消去して繋ぎ直す。一応、targetのnextもNULLにしている。freeすることがほとんどだろうが、違うリストの末尾に組み込むときのため。といっても予備で律儀に追加の関数でも初期化している。tmp->next == targetとしているのは、単方向リストゆえに、抜き出す構造体のひとつ前の構造体に戻れず、ひとつ前の構造体のnextを書き換えられため。
void remove_from_list_vertex(struct vertex **head, struct vertex *target)
{
    struct vertex *tmp ;

    if(*head == target)
        *head = (*head)->next ;
    for(tmp = *head ; tmp != NULL ; tmp = tmp->next)
        if(tmp->next == target)
            tmp->next = target->next ;

    target->next = NULL ;
}


















// //.*\nけして、vertex->jobにしてコピペしただけ。


void append_to_tail_job(struct job **head, struct job *new_element)
{
        if(*head == NULL)
    {
        *head = new_element ;
        new_element->next = NULL ;
    }
        else
    {
        get_tail_job(*head)->next = new_element ;
        new_element->next = NULL ;
    }
}

struct job *get_tail_job(struct job *head)
{
    for( ; head->next != NULL ; head = head->next) ;
    return head ;
}

unsigned int length_job(struct job *head)
{
    unsigned int i = 0 ;
    for( ; head != NULL ; head = head->next)
        i++ ;

    return i ;
}

void free_list_job(struct job **head)
{
    struct job *tmp ;

    while(*head != NULL)
    {
        tmp = *head ;
        *head = (*head)->next ;
        free(tmp) ;
    }
}

void remove_from_list_job(struct job **head, struct job *target)
{
    struct job *tmp ;

    if(*head == target)
        *head = (*head)->next ;
    for(tmp = *head ; tmp != NULL ; tmp = tmp->next)
        if(tmp->next == target)
            tmp->next = target->next ;

    target->next = NULL ;
}



void append_to_tail_adjacency_list(struct adjacency_list **head, struct adjacency_list *new_element)
{
        if(*head == NULL)
    {
        *head = new_element ;
        new_element->next = NULL ;
    }
        else
    {
        get_tail_adjacency_list(*head)->next = new_element ;
        new_element->next = NULL ;
    }
}

struct adjacency_list *get_tail_adjacency_list(struct adjacency_list *head)
{
    for( ; head->next != NULL ; head = head->next) ;
    return head ;
}

unsigned int length_adjacency_list(struct adjacency_list *head)
{
    unsigned int i = 0 ;
    for( ; head != NULL ; head = head->next)
        i++ ;

    return i ;
}

void free_list_adjacency_list(struct adjacency_list **head)
{
    struct adjacency_list *tmp ;

    while(*head != NULL)
    {
        tmp = *head ;
        *head = (*head)->next ;
        free(tmp) ;
    }
}

void remove_from_list_adjacency_list(struct adjacency_list **head, struct adjacency_list *target)
{
    struct adjacency_list *tmp ;

    if(*head == target)
        *head = (*head)->next ;
    for(tmp = *head ; tmp != NULL ; tmp = tmp->next)
        if(tmp->next == target)
            tmp->next = target->next ;

    target->next = NULL ;
}



void append_to_tail_edges(struct edges **head, struct edges *new_element)
{
        if(*head == NULL)
    {
        *head = new_element ;
        new_element->next = NULL ;
    }
        else
    {
        get_tail_edges(*head)->next = new_element ;
        new_element->next = NULL ;
    }
}

struct edges *get_tail_edges(struct edges *head)
{
    for( ; head->next != NULL ; head = head->next) ;
    return head ;
}

unsigned int length_edges(struct edges *head)
{
    unsigned int i = 0 ;
    for( ; head != NULL ; head = head->next)
        i++ ;

    return i ;
}

void free_list_edges(struct edges **head)
{
    struct edges *tmp ;

    while(*head != NULL)
    {
        tmp = *head ;
        *head = (*head)->next ;
        free(tmp) ;
    }
}

void remove_from_list_edges(struct edges **head, struct edges *target)
{
    struct edges *tmp ;

    if(*head == target)
        *head = (*head)->next ;
    for(tmp = *head ; tmp != NULL ; tmp = tmp->next)
        if(tmp->next == target)
            tmp->next = target->next ;

    target->next = NULL ;
}