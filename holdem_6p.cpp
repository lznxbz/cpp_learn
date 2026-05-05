#include <iostream>
#include <array>
#include <string>
#include <list>
#include <stdexcept>
#include <map>
using namespace std;

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
/* 
usage: holdem_6p #players hole0 hole1 [community0 ...] 
*/

list<int> card_pile;
void init_card_pile() {
    for (int i = 0; i < 36; i++) {
        card_pile.push_back(i);
    }
}

array<array<int,2>,8> hole_cards;
array<int,5> community_cards;
void dealing_cards(const int n_people, const array<int, 7>& known_cards, int n_community_cards) {
    for (int i = 0; i < n_community_cards; i++) {
        community_cards[i] = known_cards[i + 2];
    }
    hole_cards[0][0] = known_cards[0]; hole_cards[0][1] = known_cards[1];
}

int get_card_index(const array<char,2>& card_v) {
    int suits_index;
    switch (card_v.at(1)) {
        case 'c':
            suits_index = 0;
            break;
        case 'd':
            suits_index = 1;
            break;
        case 'h':
            suits_index = 2;
            break;
        case 's':
            suits_index = 3;
            break;
        default:
            throw invalid_argument("suits must be one of Clubs, Diamonds, Hearts, Spades. ");
    }

    int name_index;
    if (card_v.at(0) >= '6' && card_v.at(0) <= '9') {
        name_index = card_v.at(0) - '6'; // '6' => 0
    } else {
        switch (card_v.at(0)) {
            case 'x':
                name_index = 10 - 6;
                break;
            case 'j':
                name_index = 11 - 6;
                break;
            case 'q':
                name_index = 12 - 6;
                break;
            case 'k':
                name_index = 13 - 6;
                break;
            case 'a':
                name_index = 14 - 6;
                break;
            default:
                throw invalid_argument("card value must be one of 6-9, x, j, k, a");
        }
    }
    return 4 * name_index + suits_index;
}


array<char,2> get_card_value(const int card_index) {
    int name_index = card_index / 4;
    int suit_index = card_index % 4;
    array<char,2> card_value;

    const map<int,char> suit_map = {
        {0, 'c'},
        {1, 'd'},
        {2, 'h'},
        {3, 's'},
    };
    card_value[1] = suit_map.at(suit_index);

    const map<int,char> card_value_map = {
        {4, 'x'},
        {5, 'j'},
        {6, 'q'},
        {7, 'k'},
        {8, 'a'}
    };
    if (name_index < 4) {
        card_value[0] = '6' + name_index;
    } else {
        card_value[0] = card_value_map.at(card_index);
    }

    return card_value;
}







int main(int argc, char* argv[]) {
    array<int,7> cards_arr;

    // Handle invalid input; Read inputs, convert data type
    if (argc < 4 || argc > 9) {
        throw invalid_argument("args must include number of players, hole card 0, and hole card 1." 
            "And the number of community cards should be less or equal than five.");
    }

    int n_people = stoi(argv[1]);
    if (n_people > 8) {
        throw invalid_argument("too many people.");
    }

    for (int i = 2; i < argc; i++) {
        string arg_tmp = argv[i];
        if (arg_tmp.size() != 2) {
            throw invalid_argument("input hole card must be char[2]");
        }
        array<char,2> arg_tmp_char = {arg_tmp[0], arg_tmp[1]};
        cards_arr[i-2] = get_card_index(arg_tmp_char);
    }
    int n_community_cards = argc - 4;




    






    return 0;
}

