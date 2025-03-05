#include <iostream>
using namespace std;

struct A {
    A() { cout << "In constructor of A" << endl; }
    ~A() { cout << "In destructor of A" << endl; }
};

struct B {
    B() { cout << "In constructor of B" << endl; }
    ~B() { cout << "In destructor of B" << endl; }
};

void f() {
    std::cout << "In f()\n";
    throw("Exception msg from f()");
    std::cout << "Out of f()\n"; // This line will not be executed
}

void g() {
    std::cout << "I am in g\n";
    A _a;
    B _b;
    f();
    std::cout << "Out of g\n"; // This line will not be executed if f() throws an exception
}

int main() {
    try {
        cout << "In try block of main()" << endl;
        g();
    }
    catch (const char* e) {
        cout << "Exception: " << e << endl;
    }
    catch (...) {
        cout << "Some exception caught in main()" << endl;
    }

    cout << "Resume execution of main()" << endl;
    return 0;
}
