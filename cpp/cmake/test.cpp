#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
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

void test1() {
    std::cout << "test" << std::endl;
    MYLOG("GGGG: %d", 2);
    std::vector<int> v;
    f2(v);
    std::cout << "test1 : " << v.size() << std::endl;
}

std::mutex mtx;
std::condition_variable cv;

void t1() {
    {
        std::lock_guard<std::mutex> lk(mtx);
        std::cout << "t1 0" << std::endl;
    }
    std::cout << "t1 0A" << std::endl;
    cv.notify_one();
    std::cout << "t1 0B" << std::endl;

    {
        std::unique_lock<std::mutex> lk(mtx);
        std::cout << "t1 1A" << std::endl;
        cv.wait(lk);
        std::cout << "t1 1" << std::endl;
        lk.unlock();
        cv.notify_one();
    }
}

void t2() {
    {
        std::unique_lock<std::mutex> lk(mtx);
        std::cout << "t2 0" << std::endl;
        cv.wait(lk);
        std::cout << "t2 1" << std::endl;
        lk.unlock();
        cv.notify_one();
    }

    {
        std::unique_lock<std::mutex> lk(mtx);
        std::cout << "t2 2" << std::endl;
        cv.wait(lk);
        std::cout << "t2 3" << std::endl;
        lk.unlock();
    }
}

void test2() {
    std::thread helper2(t2);
    std::thread helper1(t1);

    std::cout << "waiting for helpers to finish..." << std::endl;
    helper1.join();
    helper2.join();
 
    std::cout << "done!\n";
}

int main()
{
    // test1();
    test2();
    return 0;
}
