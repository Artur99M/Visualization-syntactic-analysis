#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        exit(1);
    }
    const std::string dir       = argv[1];
    const std::string file      = argv[2];
    const std::string exec_file = dir + "/build/VSA.out";
    const std::string real      = dir + "/tests/test.txt";
    const std::string ref       = file + ".ans";
    // execl(exec_file.c_str(), "0<", file.c_str(), ">", real.c_str(), NULL);
    system ((exec_file + " 0< " + file + " > " + real + " 2>&1").c_str());

    std::ifstream realf {real};
    std::ifstream reff  {ref};

    struct stat buff;
    stat(ref.c_str(), &buff);
    size_t sizetext = (size_t) buff.st_size;
    char* reffstr = reinterpret_cast<char*>(operator new(sizetext));
    reff.read(reffstr, sizetext);

    stat(real.c_str(), &buff);
    sizetext = (size_t) buff.st_size;
    char* realfstr = reinterpret_cast<char*>(operator new(sizetext));
    realf.read(realfstr, sizetext);

    system (("rm " + real).c_str());

    return !(std::string(realfstr) == reffstr);
}
