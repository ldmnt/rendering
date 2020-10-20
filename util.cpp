#include "util.h"
#include <sstream>
#include <fstream>

using namespace std;

namespace util {
    string readFile(const char* filePath) {
        ifstream file;
        file.exceptions(ifstream::failbit | ifstream::badbit);
        try {
            file.open(filePath);
            stringstream stream;
            stream << file.rdbuf();
            file.close();
            return stream.str();
        }
        catch (ifstream::failure& e) {
            fprintf(stderr, "IO error: %s\n", e.what());
            return "";
        }
    }
}
