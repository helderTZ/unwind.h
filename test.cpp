#include "unwind.h"
#include <stdlib.h>

void aaa() {
    int* a = nullptr;
    *a = 10;
}

void bbb() {
    aaa();
} 

void ccc() {
    bbb();
}

int main() {
    ccc();

    return 0;
}