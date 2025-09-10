#include <iostream>
#include <vector>
#include "AnalyzerLibrary/analyzer.h"
#include "AnalyzerLibrary/src/Results.h"
using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2) {
        std::cout <<"Not enough arguments " <<std::endl;
        return 0;
    }
    Analyzer analyzer;
    std::vector<Result> results;
    Status s = analyzer.execute(argv[1], results);

    std::cout << "File name : " <<argv[1] <<", status = " <<s <<std::endl;
    return 0;
}
