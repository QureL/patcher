#include "patcher.h"
#include <stdio.h>

int larger_than_ten(int arg1)
{
    if (arg1 > 10)
        return 1;
    else
        return 0;
}

int mock(int arg)
{
    return 999;
}

int test_mock_simple_function()
{
    int ret;
    ret = exchange_addr(larger_than_ten, mock);
    if (ret == 0)
    {
        fprintf(stdout, "mock succcess\n");
    }
    else
    {
        fprintf(stdout, "mock succcess\n");
        return -1;
    }
    ret = larger_than_ten(90);
    fprintf(stdout, "%d\n", ret);
    return 0;
}

int my_printf(const char *__restrict __fmt, ...)
{
    fprintf(stdout, "in %s\n", __func__);
    return 1;
}

int test_mock_libc_function()
{
    int ret;
    ret = exchange_addr(printf, my_printf);
    if (ret == 0)
    {
        fprintf(stdout, "mock succcess\n");
    }
    else
    {
        fprintf(stdout, "mock succcess\n");
        return -1;
    }
    printf("hello");
    return 0;
}

int main()
{
    test_mock_libc_function();
}