#include <iostream>
#include <stdarg.h>

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

int main()
{
    std::cout << "test" << std::endl;
    MYLOG("GGGG: %d", 2);
    return 0;
}
