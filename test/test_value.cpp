#include "util_value.hpp"
#include "callback_test.h"
#include <iostream>

using namespace tiny_utils;

TEST_REG(value)
{
    {
        Value val(10);
        std::cout << val << std::endl;    
    }

    {
        Value val(-1);
        std::cout << val << std::endl;    
    }
        {
        Value val("123");
        std::cout << val << std::endl;    
    }
        {
        Value val(1.1);
        std::cout << val << std::endl;    
    }
        {
        Value val(true);
        std::cout << val << std::endl;    
    }
        {
         string s = "jjj";
        Value val(s);
        std::cout << val << std::endl;    
    }
    return 0;
}
