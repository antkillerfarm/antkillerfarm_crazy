#include <iostream>
#include <stdarg.h>
#include <vector>

int MYLOG(const char *fmt, ...)
{
    int bytes = 0;
    if (const char *env_p = std::getenv("ENABLE_MYLOG"))
    {
        if (atoi(env_p))
        {
            va_list arg_ptr;
            char format[256];
            sprintf(format, "[%s:%d] %s \n", __FUNCTION__, __LINE__, fmt);

            va_start(arg_ptr, fmt);
            bytes = vprintf(format, arg_ptr);
            va_end(arg_ptr);
        }
    }

    return bytes;
}


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
    std::cout << "test" << std::endl;
    MYLOG("GGGG: %d", 2);
    std::vector<int> v;
    f2(v);
    std::cout << "test1 : " << v.size() << std::endl;
    return 0;
}
