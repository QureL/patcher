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
    int ret = 0;

    patcher *p = new_patcher(larger_than_ten);
    if (!p)
        return -1;
    ret = p->patch(p, mock);
    if (ret == -1)
    {
        fprintf(stdout, "mock failed\n");
        goto out;
    }

    printf("%d\n", larger_than_ten(0));
out:
    p->recover(p);
    release_patcher(p);
    return ret;
}

int my_printf(const char *__restrict __fmt, ...)
{
    fprintf(stdout, "in %s\n", __func__);
    return 1;
}

int test_mock_libc_function()
{
    int ret = 0;

    patcher *p = new_patcher(printf);
    if (!p)
        return -1;
    ret = p->patch(p, my_printf);
    if (ret == -1)
    {
        fprintf(stdout, "mock failed\n");
        goto out;
    }

    printf("hello");
out:
    p->recover(p);
    release_patcher(p);
    return ret;
}

int main()
{
    test_mock_libc_function();
    test_mock_simple_function();
}