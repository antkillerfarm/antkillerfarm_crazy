#include <algorithm>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/topological_sort.hpp>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

void test1() {
  using namespace boost;
  {
    typedef adjacency_list<vecS, vecS, undirectedS> Graph;

    Graph G;
    add_edge(0, 1, G);
    add_edge(1, 4, G);
    add_edge(4, 0, G);
    add_edge(2, 5, G);

    std::vector<int> component(num_vertices(G));
    int num = connected_components(G, &component[0]);

    std::vector<int>::size_type i;
    cout << "Total number of components: " << num << endl;
    for (i = 0; i != component.size(); ++i)
      cout << "Vertex " << i << " is in component " << component[i] << endl;
  }
}

void test2() {
  using namespace boost;
  {
    typedef adjacency_list<vecS, vecS, directedS> Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    using Pair = std::pair<int, int>;

    std::vector<Pair> edges{
        Pair(0, 1), Pair(2, 4), Pair(2, 5),
        Pair(0, 3), Pair(1, 4), Pair(4, 3)};
    Graph G(edges.begin(), edges.end(), 6);

    typedef std::vector<Vertex> container;
    container c;
    topological_sort(G, std::back_inserter(c));

    cout << "A topological ordering: ";
    for (container::reverse_iterator ii = c.rbegin(); ii != c.rend(); ++ii)
      cout << *ii << " ";
    cout << endl;
  }
}

int main(int, char *[]) {
  test1();
  test2();
  return 0;
}