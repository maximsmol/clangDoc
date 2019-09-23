// header.hpp

#include <string>

class MyClass {
  public:
    int field;
    virtual void method() const = 0;

    /* test again */

    static const int static_field;
    static int static_method();

    // test

    std::string str = "abc";
};
