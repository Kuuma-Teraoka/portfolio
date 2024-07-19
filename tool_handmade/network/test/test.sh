for i in `ls`
do
echo -n "$i"
echo -n ", "
echo -n `clustering_coefficient < $i`
echo -n ", "
echo -n `average_shortest_path_length < $i`
echo -n ", "
echo -n ` degree_correlation < $i`
echo -n ", "
echo -n `average_degree < $i`
echo ""
done