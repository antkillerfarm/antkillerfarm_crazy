#include <iostream>
#include <tuple>
#include <utility>
#include <vector>
#include<string>
#include <map>

void test1()
{
  auto tuple = std::make_tuple(1, 'A', "test");
  std::cout << std::get<0>(tuple) << std::endl;
  std::cout << std::get<1>(tuple) << std::endl;
  std::cout << std::get<2>(tuple) << std::endl;
}

template<std::size_t... I>
std::vector<std::size_t> make_index_vector(std::index_sequence<I...>)
{
  return {I...};
}

void test2()
{
  auto vec = make_index_vector(std::make_index_sequence<10>());
  for(auto i : vec) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;
}

void test3()
{
  using MyMap = std::map< std::string,  std::string>;
  MyMap my_map;
  auto emplaced = my_map.emplace(std::string("Ali"), std::string("Jack Ma"));
  std::cout << emplaced.first->first << " => " <<emplaced.first->second <<std::endl;
}

int main()
{
  test1();
  test2();
  test3();
}
