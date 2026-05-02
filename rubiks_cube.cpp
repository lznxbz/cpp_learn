//
// Created by Lzn-mac1 on 2026-04-21.
//

#include "rubiks_cube.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
using namespace std;

using Row = char[9];
// const char rbk_ini[6] = {0b000, 0b001, 0b010, 0b011, 0b100, 0b101}; // 0bxxy: xx=axes, yy=sides
/*
{face , top, right, bottom, left, back}
{center, top, right, bottom, left, top-right, bottom-right, bottom-left, top-left} <=> map_positions
*/

const char rbk_ini[6] = {'1', 'a', 'b', 'c', 'd', '2'};
const char BG = ' ';
const map<char,string> color_map {
    {'1', "\033[41m"},
    {'a', "\033[42m"},
    {'b', "\033[43m"},
    {'c', "\033[44m"},
    {'d', "\033[45m"},
    {'2', "\033[47m"},
    {0, "  "},
};
const char map_positions[] = {8, 1, 5, 4, 0, 2, 7, 3, 6};

string manual = "Start. Input: (don't input more than 3 chars) \n"
            "move/rotate Face Action: mFA; Face - 1, a, b, c, d, 2; Action - l=left, r=right, f=forward, b=backward, i=inverse\n"
            "turn to Face: ttF; Face - 1, a, b, c, d, 2;\n"
            "qqq to quit; iii to reset/initialize; rrr to randomize the rubik;\n"
            "? to show manual; l to show/hide face label;";



char* prbk_tmp = new char[9];


// Use array pointer to go through the 2D array
void ini_rbk(Row* rbk) {
    for (char i = 0; i < 6; i++) {
        for (char j = 0; j < 9; j++) {
            rbk[i][j] = rbk_ini[i];
        }
    }
}


void form_out_arr (char (*pout)[18], char* prbk, char i) {
    char r_shift;
    char d_shift;
    switch (i) {
        case 0: // front 0=xf
            r_shift = 5;
            d_shift = 5;
            break;
        case 2: // right
            r_shift = 9;
            d_shift = 5;
            break;
        case 1: // top
            r_shift = 5;
            d_shift = 1;
            break;
        case 5: // back
            r_shift = 14;
            d_shift = 5;
            break;
        case 4: // left
            r_shift = 1;
            d_shift = 5;
            break;
        case 3: // bottom
            r_shift = 5;
            d_shift = 9;
            break;
        default:
            break;
    }

    // convert {8, 1, 5, 4, 0, 2, 7, 3, 6} to {0, 1, 2, 3, 4, 5, 6, 7, 8}
    
    for (char j = 0; j < 9; j++) {
        char index = map_positions[j];
        prbk_tmp[j] = prbk[i * 9 + index];
    }
    // fill in out_array
    for (char j = 0; j < 9; j++) {
        char rowi = j / 3 + d_shift;
        char indexi = j % 3 + r_shift;
        pout[rowi][indexi] = prbk_tmp[j];
    }
    
}


// Use flatten pointer to go through the 2D array
void print_out(char* prbk, bool show_label) {
    char output[13][18] = {};

    // Write output array
    for (char i = 0; i < 6; i++) {
        form_out_arr(output, prbk, i);
    }

    // Print output array
    char counters[] = {3, 3, 3, 3, 3, 3};
    char l_label[6] = {'a', 'd', '1', 'b', '2', 'c'};
    char l_empty[6] = {' ', ' ', ' ', ' ', ' ', ' '};
    char l_show[6];
    if (show_label) {
        memcpy(l_show, l_label, 6);
    } else {
        memcpy(l_show, l_empty, 6);
    }

    for (auto & i : output) {
        char counter3 = 0;
        char counter4 = 0;
        for (char val : i) {
            while (counters[counter4] == 0) {counter4++;} // skip empty paint
            char label = l_show[counter4];
            if (val != 0) {
                cout << color_map.at(val) << label << " \033[0m ";
                counter3++; // paint on canva
            } else {
                cout << "   ";
            }
            if (counter3 == 3) {
                counter3 = 0;
                counters[counter4]--; // consume paint
                counter4++;
            }
        }
        cout << "\n" << endl;
    }
    cout << endl;
}



void elementary_move(Row* prbk) {
    // center front face: 1 -> 1*
    prbk_tmp[0] = prbk[0][0];
    for (char i = 1; i < 4; i++) {
        prbk_tmp[i + 1] = prbk[0][i];
        char j = i + 4;
        prbk_tmp[j + 1] = prbk[0][j];
    }
    prbk_tmp[1] = prbk[0][4];
    prbk_tmp[5] = prbk[0][8];

    copy(prbk_tmp, prbk_tmp+9, prbk[0]);

    // sides: a -> b -> c -> d -> a; 2 -> 2
    char modify_side[] = {2,5,6};
    // save face-d temporarily
    for (auto & ii : modify_side) {
        prbk_tmp[ii] = prbk[4][ii];
    }
    // for each side
    for (char i = 4; i > 0; i--) {
        for (char & ii : modify_side) {
            char jj = ii - 1;
            if (jj == 0) {jj = 4; goto lable_1;}
            if (jj == 4) {jj = 8;}
            lable_1:
                prbk[i][ii] = prbk[i - 1][jj];
            if (i == 1) { // copy temporarily saved side-d to side-a
                prbk[i][ii] = prbk_tmp[jj];
            }
            ii = jj;
        }
    }

    
}


void elementary_right(Row* prbk) {
    // Sides
    memcpy(prbk_tmp, prbk[0], 9);
    memcpy(prbk[0], prbk[2], 9);
    memcpy(prbk[2], prbk[5], 9);
    memcpy(prbk[5], prbk[4], 9);
    memcpy(prbk[4], prbk_tmp, 9);

    // Top rot (could be replaced by rot_face_cw(prbk[1], 1))
    memcpy(prbk_tmp, prbk[1], 9);
    for (char i = 1; i <5; i++) {
        char j = i + 1;
        if (j == 5) { j = 1; }
        prbk[1][j] = prbk_tmp[i];
        prbk[1][j+4] = prbk_tmp[i+4];
    }

    // Bottom (could be replaced by rot_face_cw(prbk[3], 3))
    memcpy(prbk_tmp, prbk[3], 9);
    for (char i = 1; i <5; i++) {
        char j = i + 1;
        if (j == 5) { j = 1; }
        prbk[3][i] = prbk_tmp[j];
        prbk[3][i+4] = prbk_tmp[j+4];
    }
}


void rot_face_cw(char* prbk, char n_rot) {
    for (char ii = 0; ii < n_rot; ii++) {
        memcpy(prbk_tmp, prbk, 9);
        for (char i = 1; i <5; i++) {
            char j = i + 1;
            if (j == 5) { j = 1; }
            prbk[j] = prbk_tmp[i];
            prbk[j+4] = prbk_tmp[i+4];
        }
    }
}


void elementary_top(Row* prbk) {
    // change face
    memcpy(prbk_tmp, prbk[0], 9);
    memcpy(prbk[0], prbk[1], 9);
    memcpy(prbk[1], prbk[5], 9);
    memcpy(prbk[5], prbk[3], 9);
    memcpy(prbk[3], prbk_tmp, 9);

    // rot in each face
    rot_face_cw(prbk[1], 2);
    rot_face_cw(prbk[2], 3);
    rot_face_cw(prbk[4], 1);
    rot_face_cw(prbk[5], 2);
}


void elementary_roll_cw(Row* prbk) {
    elementary_right(prbk);
    elementary_top(prbk);
    for (char i = 0; i < 3; i++) {
        elementary_right(prbk);
    }
}


void turn_to(Row* prbk, char tt_face) {
    switch (tt_face) {
        case '1':
            break;
        case 'a':
            elementary_top(prbk);
            break;
        case 'b':
            elementary_right(prbk);
            break;
        case 'c':
            elementary_top(prbk); elementary_top(prbk); elementary_top(prbk);
            break;
        case 'd':
            elementary_right(prbk); elementary_right(prbk); elementary_right(prbk);
            break;
        case '2':
            elementary_right(prbk); elementary_right(prbk);
            break;
        default:
            cout << "No such face: " << tt_face << endl;
            break;
    }
}


void basic_move(Row* prbk, char turn_n, void (*fxn)(Row*), char rot_n) {
    char turn_back = 4 - turn_n;
    if (turn_n < 2) {
        for (char i = 0; i < turn_n; i++) {fxn(prbk);} // turn to face
        for (char i = 0; i < rot_n; i++) {elementary_move(prbk);} // rot
        for (char i = 0; i < turn_back; i++) {fxn(prbk);} // turn it back
    } else {
        rot_n = 4 - rot_n;
        for (char i = 0; i < turn_n; i++) {fxn(prbk);}
        for (char i = 0; i < rot_n; i++) {elementary_move(prbk);}
        for (char i = 0; i < turn_back; i++) {fxn(prbk);}
    }
}



bool move (Row* prbk, char* input) {
    char turn_n;
    void (*fxn)(Row*);
    switch (input[1]) {
        case '1':
            turn_n = 0; fxn = elementary_right;
            break;
        case 'a':
            turn_n = 1; fxn = elementary_top;
            break;
        case 'b':
            turn_n = 1; fxn = elementary_right;
            break;
        case 'c':
            turn_n = 3; fxn = elementary_top;
            break;
        case 'd':
            turn_n = 3; fxn = elementary_right;
            break;
        case '2':
            turn_n = 2; fxn = elementary_right;
            break;
        default:
            return false;
    }

    char rot_n;
    switch (input[2]) {
        case 'r':
        case 'f':
            rot_n = 1;
            break;
        case 'l':
        case 'b':
            rot_n = 3;
            break;
        case 'i':
            rot_n = 2;
            break;
        default:
            return false;
    }

    if (input[1] == 'a' || input[1] == 'c') {rot_n = 4 - rot_n; cout << (int)rot_n << endl;}
    basic_move(prbk, turn_n, fxn, rot_n);
    return true;
}


void randomize(Row* prbk) {
    for (char i = 0; i < 100; i++) {
        char rand_n = rand() % 3;
        switch (rand_n) {
            case 0:
                elementary_move(prbk);
                break;
            case 1:
                elementary_right(prbk);
                break;
            case 2:
                elementary_top(prbk);
                break;
        }
    }
}



int main() {
    // some constants and variables
    const char qqq[] = {'q','q','q', 0};
    const char iii[] = {'i','i','i', 0};
    const char rrr[] = {'r','r','r', 0};
    bool show_lable = true;

    // initialization
    char rbk[6][9] = {}; // axis-side-number => 3 digits number (e.g. xf0 => 000)
    ini_rbk(&rbk[0]);

    cout << manual << endl;
     char input[4];

    bool skip_loop = false;
    // main loop
    while (true) {
        if (skip_loop == true) {
            skip_loop = false;
            continue;
        }

        // Print out rubik cube
        print_out(&rbk[0][0], show_lable);

        // Read input
        cout << "Input: ";
        cin >> setw(4) >> input;
        if (input[3] != 0) {
            cout << "Please input 1-3 characters. " << endl;
            skip_loop = true;
        }

        bool valid_input;
        switch (*input) {
            case 't':
                if (input[1] == 't') {
                    turn_to(rbk, input[2]);
                    break;
                }
                goto label_default;
            case 'm':
                valid_input = move(rbk, input);
                if (valid_input) {break;}
                goto label_default;
            case 'r':
                if (memcmp(input, &rrr, 4) ==  0) {
                    randomize(rbk);
                    break;
                }
                goto label_default;
            case 'e':
                elementary_move(&rbk[0]);
                break;
            case 'x':
                elementary_right(&rbk[0]);
                break;
            case 'y':
                elementary_top(rbk);
                break;
            case 'q':
                if (memcmp(input, &qqq, 4) == 0) {
                    cout << "QUIT!" << endl;
                    return 0;
                }
                goto label_default;
            case 'i':
                if (memcmp(input, &iii, 4) == 0) {
                    cout << "Initialize/Reset!" << endl;
                    ini_rbk(rbk);
                    break;
                }
                goto label_default;
            case '?':
                cout << manual << endl;
                break;
            case 'l':
                show_lable = !show_lable;
                break;
            default:
                label_default:
                cout << "Invalid input => No action. " << endl;
                break;
        }





        // // change view
        // if (*input == 'c') {
        //     change_view(&rbk[0], input+1);
        //     continue;
        // }
        //
        // // operation
        // if (*input >= 'x' && *input <= 'z') {
        //     char status = operation(&rbk[0], input);
        //     if (status == 'p') {
        //         cout << "invalid operation. Input again" << endl;
        //     }
        //     continue;
        // }
        // if (*input == 'e') {
        //     elementary_move(&rbk[0]);
        //     continue;
        // }
        //
        // // initialize/reset
        // if (*input == 'i') {
        //     cout << "rubik cube initialized. " << endl;
        //     ini_rbk(&rbk[0]);
        // }
        // // show history
        //
        //
        // // qqq to quit program
        // if (*input == 'q' ) {
        //     cout << "QUIT" << endl;
        //     break;
        // }

    }
    return 0;
}

