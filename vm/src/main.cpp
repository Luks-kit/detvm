#include "detvm.hpp"
#include "ops.hpp"
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    using namespace detvm;

    if (argc < 2) {
        std::cerr << "Usage: vm <input.detbc>\n";
        return 1;
    }

    std::string filename = argv[1];

    VM vm;

    vm.loadProgram(assembler::readFile(filename));

    vm.run();




    std::cout << "[vm] Execution complete.\n";
    return 0;
}
