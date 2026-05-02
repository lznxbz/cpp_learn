//
// Created by Lzn-mac1 on 2026-04-21.
//

#include <iostream>
#include <ctime>
#include <unistd.h>

using namespace std;

#define OUT cout << hex << "fib(" << n << ") = " << v << endl;

// Store! fibonacci in memory
unsigned long int fibonacci(unsigned int n) {
    clock_t start_c = clock();

    unsigned long int v; // result value
    unsigned long int fib_array[1024] = {0,1,}; // 10000 * 4 = 40000 bytes ~ 40 kB (int = short int: 4 bytes)

    unsigned long int* pext = fib_array;

    unsigned int m; // For external memory address (how much exceed)

    if (n <= 1) {
        v = n;

    } else if (n < 1024) {
        for (unsigned int i = 2; i <= n; i++) {
            fib_array[i] = fib_array[i - 1] + fib_array[i - 2];
        }
        v = fib_array[n];

    } else {
        cout << "Big number: " << n << ">=1024. Use heap memory" << endl;
        m = n - 1023;
        n = 1023;
        pext = new unsigned long int[m];

        for (unsigned int i = 2; i <= n; i++) {
            fib_array[i] = fib_array[i - 1] + fib_array[i - 2];
        }

        pext[0] = fib_array[1022] + fib_array[1023];
        if (m == 1) {
            v = pext[0];
            goto target_out;
        }

        pext[1] = fib_array[1023] + pext[0];
        if (m == 2) {
            v = pext[1];
            goto target_out;
        }

        for (unsigned int i = 2; i < m; i++) {
            pext[i] = pext[i - 1] + pext[i - 2];
        }

        v = pext[m-1];
    }

    // Output time
    target_out:
    clock_t duration = clock() - start_c;
    cout << "Run time: " << (float)duration / CLOCKS_PER_SEC * 1000000 << " us" << endl;
    return v;
}




int main() {
    unsigned int n;
    cout << "Enter a number (0 indexed): ";
    cin >> n;


    unsigned long int v = fibonacci(n);
    OUT;

    return 0;
}



