#include "linker.hpp"
#include "reader.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {

    using namespace detvm;
    if (argc < 3) {
        std::cerr << "Usage: detld <input1.dto> [input2.dto ...] <output.dtb>\n";
        return 1;
    }

    try {
        std::vector<assembler::AssemblerResult> objects;

        // all args except the last are inputs
        for (int i = 1; i < argc - 1; ++i) {
            std::string path = argv[i];
            auto object = linker::readObject(path);
            objects.push_back(object);   
        }

        std::string output_path = argv[argc - 1];

        auto linked = linker::linkObjects(objects);
        linker::linkLabels(
        linked.code,
        linked.label_to_pc,
        linked.unresolved,
        linked.funcs
        );
        linker::writeProgramBinary(output_path, linked);

        std::cout << "Linked " << (argc - 2) << " objects -> " << output_path << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Linking failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
