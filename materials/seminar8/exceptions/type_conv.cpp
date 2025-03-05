#include <iostream>

enum Err {
    OK,
    NO_OK
};

int main() {
    try {
        Err m = NO_OK;
        std::cout << "Error Num: " << (int)(m) << std::endl;
        throw m;
    }
    catch (int&) {
        std::cout << "int\n";
    }
    catch (Err) {
        std::cout << "Err\n";
    }

    return 0;
}
