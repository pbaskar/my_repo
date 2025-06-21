#ifndef RESULTS_H
#define RESULTS_H

enum Status{
    SUCCESS,
    FAILURE,
    FIRST_MISMATCH
};

struct Result {
    char* errorMessage;
};

#endif // RESULTS_H
