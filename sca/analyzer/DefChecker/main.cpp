#include <iostream>
#include <vector>
#include <sstream>
#include "AnalyzerLibrary/analyzer.h"
#include "AnalyzerLibrary/src/Results.h"
using namespace std;

int main(int argc, char *argv[])
{
    const char* inputFile = nullptr;
    const char* outputFilePath = nullptr;
    const char* usageMsg = "Usage: DefChecker.exe -i test\\swap_num.c -o <WritableFolder>";
    if(argc < 5) {
        std::cout << usageMsg << std::endl;
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i")==0) {
            i++;
            if (i < argc) {
                inputFile = argv[i];
            }
            else {
                std::cout << usageMsg << std::endl;
                return 0;
            }
        }
        else if (strcmp(argv[i], "-o") == 0) {
            i++;
            if (i < argc) {
                outputFilePath = argv[i];
            }
            else {
                std::cout << usageMsg << std::endl;
                return 0;
            }
        }
        else if (strcmp(argv[i], "--help") == 0) {
            std::cout << usageMsg << std::endl;
            return 0;
        }
    }
    Analyzer analyzer;
    std::vector<Result> results;
    if (!inputFile || !outputFilePath) {
        std::cout << usageMsg << std::endl;
        return 0;
    }

    Status s = analyzer.setOutputPath(outputFilePath);
    if (s == FAILURE) {
        return 0;
    }
    s = analyzer.execute(inputFile, outputFilePath, results);
    std::cout << "File name : " <<inputFile <<", status = " <<s <<std::endl;
    return 0;
}
