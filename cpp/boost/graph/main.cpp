#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

using namespace std;

int main(int , char* []) 
{
  using namespace boost;
  {
    typedef adjacency_list <vecS, vecS, undirectedS> Graph;

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
      cout << "Vertex " << i <<" is in component " << component[i] << endl;
    cout << endl;
  }
  return 0;
}