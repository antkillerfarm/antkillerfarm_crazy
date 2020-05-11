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

    std::vector<Pair> edges{Pair(0, 1), Pair(2, 4), Pair(2, 5),
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

void test3() {
  using namespace boost;
  {
    typedef property<vertex_name_t, std::string> VertexProperty;
    typedef adjacency_list<vecS, vecS, directedS, VertexProperty> Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    using Pair = std::pair<std::string, std::string>;

    Pair edges[] = {Pair("A0", "A1"), Pair("A2", "A4"), Pair("A2", "A5"),
                    Pair("A0", "A3"), Pair("A1", "A4"), Pair("A4", "A3")};

    const char *vertices[] = {"A0", "A1", "A2", "A3", "A4", "A5"};
    std::map<std::string, Vertex> indexes;

    const int nb_vertices = sizeof(vertices) / sizeof(vertices[0]);
    Graph G(nb_vertices);

    for (int i = 0; i < nb_vertices; i++) {
      boost::put(vertex_name_t(), G, i,
                 vertices[i]); // set the property of a vertex
      indexes[vertices[i]] =
          boost::vertex(i, G); // retrives the associated vertex descriptor
    }
    for (int i = 0; i < sizeof(edges) / sizeof(edges[0]); i++) {
      boost::add_edge(indexes[edges[i].first], indexes[edges[i].second], G);
    }
    typedef std::vector<Vertex> container;
    container c;
    topological_sort(G, std::back_inserter(c));

    cout << "A topological ordering: ";
    for (container::reverse_iterator ii = c.rbegin(); ii != c.rend(); ++ii) {
      cout << boost::get(vertex_name_t(), G, *ii) << " ";
    }
    cout << endl;
  }
}

void test4() {
  using namespace boost;
  {
    typedef adjacency_list<vecS, vecS, directedS> Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    using Pair = std::pair<std::string, std::string>;

    Pair edges[] = {Pair("A0", "A1"), Pair("A2", "A4"), Pair("A2", "A5"),
                    Pair("A0", "A3"), Pair("A1", "A4"), Pair("A4", "A3")};

    const char *vertices[] = {"A0", "A1", "A2", "A3", "A4", "A5"};
    const int nb_vertices = sizeof(vertices) / sizeof(vertices[0]);
    Graph G(nb_vertices);
    std::map<std::string, int> s2i_table;
    std::map<int, std::string> i2s_table;

    for (int i = 0; i < nb_vertices; i++) {
      s2i_table[vertices[i]] = i;
      i2s_table[i] = vertices[i];
    }
    for (int i = 0; i < sizeof(edges) / sizeof(edges[0]); i++) {
      boost::add_edge(s2i_table[edges[i].first], s2i_table[edges[i].second], G);
    }
    typedef std::vector<Vertex> container;
    container c;
    topological_sort(G, std::back_inserter(c));

    cout << "A topological ordering: ";
    for (container::reverse_iterator ii = c.rbegin(); ii != c.rend(); ++ii)
      cout << i2s_table[*ii] << " ";
    cout << endl;
  }
}

int main(int, char *[]) {
  test1();
  test2();
  test3();
  test4();
  return 0;
}