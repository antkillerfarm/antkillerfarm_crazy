

#include <iostream>
#include <iterator>
#include <tuple>
#include <set>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include <numeric>
#include <algorithm>

/**
* # OP1
* 
* OP1 is a OP.
*/
void test1()
{
  auto tuple = std::make_tuple(1, 'A', "test");
  std::cout << std::get<0>(tuple) << std::endl;
  std::cout << std::get<1>(tuple) << std::endl;
  std::cout << std::get<2>(tuple) << std::endl;
}

/**
* # OP2
* 
* OP2 is a OP.
*/

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
  std::iota(vec.begin(), vec.end(), 999);
  std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  std::for_each(std::begin(vec),std::end(vec),[](int n){std::cout<<n<<" ";});
  std::cout << std::endl;
}

void test3()
{
  using MyMap = std::map< std::string,  std::string>;
  MyMap my_map;
  auto emplaced = my_map.emplace(std::string("Ali"), std::string("Jack Ma"));
  std::cout << emplaced.first->first << " => " <<emplaced.first->second << std::endl;
}

std::pair<int, std::string> fun_tie(int a, std::string str)
{
  return std::make_pair(a, str);
}

std::tuple<int, std::string, std::string> fun_tie2(int a, std::string str, std::string str1)
{
  return std::make_tuple(a, str, str1);
}

void test4()
{
  int a;
  std::string str, str2;
  std::tie(a , str) = fun_tie(12, std::string("Pony Ma"));
  std::cout << a << "," << str << std::endl;
  std::tie(a , str, str2) = fun_tie2(21, std::string("Robin Li"), std::string("Elon Musk"));
  std::cout << a << "," << str << "," << str2 << std::endl;
}

void test5()
{
  std::set<int> s;
  s.insert(1);
  s.insert(2);
  s.insert(3);
  s.insert(1);
  std::cout << "Count of 1 ：" << s.count(1) << std::endl;
  std::cout << "Count of 4 ：" << s.count(4) << std::endl;
}

int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();
}
