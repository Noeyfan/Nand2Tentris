#include <string>
#include <iostream>
#include <filesystem>
#include "./JackTokenizer.cc"
#include "./CompilationEngine.cc"
using namespace std;

void printTokens(JackTokenizer& jt) {
    cout << "<tokens>\n";
    while(jt.hasMoreTokens()) {
        cout << jt.getXml(NO_CHECK) << "\n";
        jt.advance();
    }
    cout << "</tokens>\n";
}

void processSingleFile(const string& path) {
    // Some crappy filename parsing
    size_t lastDot = path.find_last_of(".");
    size_t lastSlash = path.find_last_of("/");
    size_t start = 0;

    // string filename = path.substr(start, lastDot - start);
    if (lastSlash == -1) {
        start = 0;
    } else {
        start = lastSlash+1;
    }

    std::ifstream input(path);
    std::ofstream output(path.substr(0, lastDot) + ".out.xml");

    // uncomment to output token xml
    // JackTokenizer jt(input);
    // printTokens(jt);

    CompilationEngine ce(input, output);
    ce.compileClass();
}

void processDirectory(const string& path) {
    string ext(".jack");
    for (auto &p : filesystem::recursive_directory_iterator(path)) {
        if (p.path().extension() == ext) {
            processSingleFile(p.path().string());
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        cout << "./a.out <filename>\n" << "./a.out <directory>\n";
        return 0;
    }

    error_code ec;
    const string path(argv[1]);
    if (filesystem::is_directory(path, ec)) {
        processDirectory(argv[1]);
    } else {
        processSingleFile(path);
    }
}
