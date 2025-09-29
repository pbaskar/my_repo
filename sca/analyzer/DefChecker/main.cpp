#include <iostream>
#include <vector>
#include "AnalyzerLibrary/analyzer.h"
#include "AnalyzerLibrary/src/Results.h"
using namespace std;

int main(int argc, char *argv[])
{
    const char* inputFile = nullptr;
    const char* outputFile = nullptr;
    const char* usageMsg = "Usage: DefChecker.exe -i \\test\\swap_num.c -o output.log";
    if(argc < 3) {
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
                outputFile = argv[i];
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
    if (!inputFile) {
        std::cout << usageMsg << std::endl;
        return 0;
    }
    if (!outputFile) { outputFile = "output.log"; }
    Status s = analyzer.execute(inputFile, outputFile, results);

    std::cout << "File name : " <<inputFile <<", status = " <<s <<std::endl;
    return 0;
}
