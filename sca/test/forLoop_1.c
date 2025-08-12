//array traversal
#include <cstdio>
void printArray(int *arr) {
    for(int i=0; i<5; i=i+1) {
        //printf("%d", arr[i]);
    }
    return;
}

void populateArray(int *arr) {
    for(int i=0; i<5; i=i+1) {
        arr[i] =i;
    }
    return;
}

int main(int argc) {
    int arr[10];
    populateArray(arr);
    printArray(arr);
}