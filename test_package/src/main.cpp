#include <exception>
#include <iostream>

auto main() -> int {
    try {
        
        return 0;
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
