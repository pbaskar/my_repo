#ifndef RESULTS_H
#define RESULTS_H

enum Status{
    SUCCESS,
    FAILURE
};

struct Result {
    char* errorMessage;
};

#endif // RESULTS_H
