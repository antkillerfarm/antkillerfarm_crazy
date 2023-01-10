#include <iostream>
#include <vector>

std::vector<int> f1() {
    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    return v;
}

void f2(std::vector<int>& v) {
    auto v1 = f1();
    v = std::move(v1);
    std::cout << "test0 : " << v.size() << std::endl;
}

int main()
{
    std::vector<int> v;
    f2(v);
    std::cout << "test1 : " << v.size() << std::endl;
    return 0;
}
