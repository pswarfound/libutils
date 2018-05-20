#include <stdio.h>
#include "util_value.hpp"
#include "callback_test.h"
#include <iostream>

using namespace tiny_utils;

TEST_REG(value)
{
    Value test;

    {
        Value val("123");
        std::cout << "string test: " << val << std::endl;    
        test = val;
    }

    std::cout << "copy test: " << test << std::endl;    

    {
        Value val(1.1);
        std::cout << "float test: " << val << std::endl;    
    }

    {
        uint64_t x = ~0;
        Value val(x);
        std::cout << "uint64 test: " << val << std::endl;    
    }

    {
        Value val(true);
        std::cout << "boolean test " << val << std::endl;    
    }
    
    return 0;
}
