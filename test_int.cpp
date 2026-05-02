#include <iostream>
#include <iomanip>
#include <bitset>
#include <cstring>
#include <bit> // std::bit_cast

using namespace std;

int abc = 123;


void array_int(){
    int a[] = {1,2,3,4,5};
    int *p = a;
    cout << p << endl;
}


int an_int(){
    float a = 10.123;
    int b = 20;

    float *pa = &a;
    int *pb = &b;

    *pb = 555;

    cout << *(pb+1) << endl;

    cout << "pointer a: " << pa << " with value: " << *pa << endl;
    cout << "pointer b: " << pb << " with value: " << *pb << endl;

    return 0;
}


int unsigned_int() {
    unsigned int ttt = -1;
    cout << ttt << endl;
    return 0;
}


char test_pointer() {
    char xxx = 100;
    cout << xxx << ":" << sizeof(xxx) << endl;
    cout << sizeof("1234") << endl;
    string s = "t";
    string* ptr = &s;
    cout << sizeof(*ptr) << endl;
    // s = "xxxxxxxx";
    cout << sizeof(*ptr) << endl;
    cout << *ptr << endl;

    return 0;
}





int* xxx() {
    static int a = 1000;
    int* pa = &a;
    return pa;
}




int* yyy() {
    static int rub[6] = {1,2,3,4,5,6}; // axis-side-number => 3 digits number (e.g. xf0 => 000)
    static int rubb[2][3] = {{3,2,1},{6,5,4}};
    cout << "  "
    << rubb << "  " << &rubb << "  " << rubb[0] << "  "
    << *rubb << " " << &rubb[0] << "  " << endl;

    cout << "Size of &rubb:    " << sizeof(&rubb) << " (pointer size)" << endl;
    cout << "Size of rubb:     " << sizeof(rubb) << " (total array size)" << endl;
    cout << "Size of rubb[0]:  " << sizeof(rubb[0]) << " (one row size)" << endl;

    cout << "--- Address math ---" << endl;
    cout << "rubb      + 1: " << (void*)(rubb + 1) << " (moved by 12 bytes)" << endl;
    cout << "rubb[0]   + 1: " << (void*)(rubb[0] + 1) << " (moved by 4 bytes)" << endl;

    cout << rubb[1][2] << " "
    << *(rubb[1]+2) << " "
    << *(*(rubb+1)+2) << " ";

    int* prub = rub;
    int* prubb = rubb[0];

    // cout << rub << "=" << prub << endl;
    // cout << rubb << "=" << prubb << endl;

    return prub;
}


void array_ptr() {

}

void prt() {
    int a = 2;
    int* p = &a;
}




int func1() {
    float a = 1.1f;
    uint32_t bits;
    char arr[4];

    memcpy(&bits, &a, sizeof(a));

    // 使用 bitset 打印 32 位二进制
    std::cout << "Float: " << a << std::endl;
    std::cout << "Binary: " << std::bitset<32>(bits) << std::endl;

    bits = 0x00610000;
    memcpy(&a, &bits, sizeof(a));
    memcpy(&arr[0], &bits, sizeof(arr));
    cout << "Binary: " << bitset<32>(bits) << endl;
    std::cout << "Float: " << a << std::endl;
    cout << arr[1] << endl;
    cout << (char)0x61 << std::endl;

    return 0;
}


void func2() {
    char input[4];
    cin >> setw(4) >> input;
    cout << (int)input[0] << endl;
    cout << (int)input[3] << endl;
}


void func3() {
    char a = 2;
    switch (a) {
        case 1:
            cout << "1" << endl;
        case 2:
            if (0) {
                cout << "2" << endl;
            }
        case 3:
            cout << "3" << endl;
        default:
            cout << "d" << endl;
    }
}


int main(){
    // int xxx = an_int();
    // cout << endl << endl;
    // array_int();

    // test_pointer();
    // int* y = yyy();
    // int rub[1000];
    // cout << *(y+2) << endl;

    // cout << abc << endl;
    // f1();
    // cout << abc << endl;

    // int a = 2;
    // if (true)
    //     a = 3;
    //     int b = 4;
    //
    // cout << a << endl;
    // cout << b << endl;


    // while (true) {
    //     char ccc[4];
    //     cout << "Input: ";
    //     cin >> setw(4) >> ccc;
    //     cout << sizeof(ccc) << endl;
    //     cout << ccc << endl;
    //     for (int i = 0; i < 4; i++) {
    //         cout << (int)ccc[i] << " ";
    //     }
    //     cout << endl;
    // }

    // int arr[10] = {1,2,3,4,5,6};
    // int *p = arr;
    // cout << &arr << " " << arr << endl;
    // cout << &p << " " << p << endl;
    //
    // int* ph = new int[10];
    // cout << ph << endl;
    //
    // for (int i = 0; i < 10; i++) {
    //     ph[i] = p[i];
    // }
    //
    // cout << ph[3] << endl;
    //
    // p = new int[10];
    // cout << p << endl;
    //
    // delete[] ph;
    // delete[] p;


    // cout << "\033[41m  \033[0m" << endl;

    // for (int i = 0; i < 5; i++) {
    //     i ++;
    //     cout << i << endl;
    // }

    // char arr[] = "abc";
    // cout << arr << endl;
    //
    // for (auto &i : arr) {
    //     i = '1';
    // }
    // cout << arr << endl;


    // func1();
    // func2();
    func3();
}


