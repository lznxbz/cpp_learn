#include <iostream>
#include <array>
#include <string>
#include <list>
#include <stdexcept>
#include <map>
#include <functional>
#include <random>
#include <thread>
#include <ctime>
#include <format>
using namespace std;

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
/* 
usage: holdem_6p #players hole0 hole1 [community0 ~ community4] 
*/


int int_pow(int base, int exponent) {
    if (exponent > 0) {
        exponent --;
        return base * int_pow(base, exponent);
    } else {
        return 1;
    }
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
                throw invalid_argument("card point must be one of 6-9, x, j, k, a");
        }
    }
    return 4 * name_index + suits_index;
}

array<char,2> get_card_point(const int card_index) {
    int name_index = card_index / 4; 
    int suit_index = card_index % 4;
    array<char,2> card_point;

    const map<int,char> suit_map = {
        {0, 'c'},
        {1, 'd'},
        {2, 'h'},
        {3, 's'},
    };
    card_point[1] = suit_map.at(suit_index);

    const map<int,char> card_point_map = {
        {4, 'x'},
        {5, 'j'},
        {6, 'q'},
        {7, 'k'},
        {8, 'a'}
    };
    if (name_index < 4) {
        card_point[0] = '6' + name_index;
    } else {
        card_point[0] = card_point_map.at(name_index);
    }

    return card_point;
}



class Holdem {
    private:
        list<int> card_pile;
        void init_card_pile();
        array<array<int,2>,8> hole_cards;
        array<int,5> community_cards;
        void dealing_cards(mt19937& gen);

    public:
        int n_players;
        int n_community_cards;
        int* pknown_cards;
        int calc_score(int hole0, int hole1);
        int n_games = 1000;
        array<double,8> probability;
        void get_probability(mt19937& gen);
        void test_game(mt19937& gen);

};



void Holdem::init_card_pile() {
    for (int i = 0; i < 36; i++) {
        card_pile.push_back(i);
    }
}

void Holdem::dealing_cards(mt19937& gen) {
    // Take out from card pile
    for (int i = 0; i < n_community_cards + 2; i++) {
        int card_i = pknown_cards[i];
        auto remove_counter = card_pile.remove(card_i);
        if (remove_counter != 1) {
            cout << remove_counter << " of cards removed!" << endl;
            throw invalid_argument("Cards out of range, or duplication of cards.");
        }
    }

    // Assign to me and community cards
    for (int i = 0; i < n_community_cards; i++) {
        community_cards[i] = pknown_cards[i + 2];
    }
    hole_cards[0][0] = pknown_cards[0]; hole_cards[0][1] = pknown_cards[1];
    
    // Take out and assign for the rest players
    for (int i = 1; i < n_players; i++) {
        for (int ii = 0; ii < 2; ii++){
            // assign a random card
            auto it_card = card_pile.begin();
            uniform_int_distribution<int> dist(0, card_pile.size() - 1);
            int p_card = dist(gen);
            advance(it_card, p_card);
            hole_cards[i][ii] = *it_card;
            card_pile.erase(it_card);
        }
    }

    // Show all of 5 community cards
    for (int i = n_community_cards; i < 5; i++) {
        auto it_card = card_pile.begin();
        uniform_int_distribution<int> dist(0, card_pile.size() - 1);
        int p_card = dist(gen);
        advance(it_card, p_card);
        community_cards[i] = *it_card;
        card_pile.erase(it_card);
    }
}




int Holdem::calc_score(int hole0, int hole1) {
    array<int, 7> full_hands;
    for (int i = 0; i < 5; i++) full_hands[i] = community_cards[i];
    full_hands[5] = hole0; full_hands[6] = hole1;

    // transform to the space of occurences
    array<int,4> suits_counter{};
    array<int,9> value_counter{};
    for (auto i : full_hands) {
        int suit_idx = i % 4;
        int value_idx = i / 4;
        suits_counter[suit_idx] ++;
        value_counter[value_idx] ++;
    }
    
    int score = 0;

    // Four a kind, score = 1e5 + max
    // Mutual exclusive for all of the rest cases. => Retrun score directly
    for (int i = 0; i < 9; i++) { // can only exist one four a kind, iteration order doesn't matter
        if (value_counter[i] == 4) return 100000 + i; // score
    }
    

    // Straight, score = 1e3 + max
    int straight_counter = 0;
    int straight_max;
    for (int i = 8; i >= 0; i--) { // high to low
        if (value_counter[i] > 0) {
            if (straight_counter == 0) straight_max = i;
            straight_counter ++;
            if (straight_counter == 5) score += 1000 + straight_max;
        } else {
            straight_counter = 0;
            if (i < 4) break; // the only straight that < 10 is 9-A, which will be included below
        }
    }
    if (value_counter[8] > 0) { // if Ace exist, then check 9-A straight
        int i = 0;
        for (i = 0; i < 4; i++) {
            if (value_counter[i] == 0) break;
        } 
        if (i == 3) score += 1003;
    }


    // (straight) flush: senario which both suits and point should be considered at the same time
    // Flush, score=1e4+max
    int flush_suit = 5;
    for (int i = 0; i < 4; i++) {
        if (suits_counter[i] >= 5) {
            flush_suit = i;
            break;
        }
    }
    if (flush_suit != 5) { // flush exist, find the biggest card
        int flush_max = 0;
        for (auto i : full_hands) {
            if (i % 4 == flush_suit) {
                int flush_i = i / 4;
                if (flush_i > flush_max) flush_max = flush_i;
            }
        }
        score += 10000 + flush_max;
    }

    // straight flush, score = 1e5 * max
    if (score > 11000) { // both straight and flush exist
        int n_flush = suits_counter[flush_suit]; // number of flush cards
        list<int> straight_list;
        for (auto i : full_hands) {
            if (i % 4 == flush_suit) straight_list.push_back(i / 4);
        }
        straight_list.sort(greater<int>()); // high to low
        straight_counter = 0;

        auto itr = straight_list.begin();
        int card_previous = *itr;
        int card_max = *itr;
        advance(itr, 1);
        for (itr; itr != straight_list.end(); ++itr) {
            if (*itr == card_previous - 1) {
                straight_counter ++;
                if (straight_counter == 4) return card_max * 100000; // straigh flush achieved
            } else {
                straight_counter = 0;
                card_max = *itr;
            }
            card_previous = *itr;
        }
    }

    if (score > 10000) return score; // return flush

    int low_score = 0;

    // Three a kind, low_score = 1e3 + max
    for (int i = 8; i >= 0; i--) {
        if (value_counter[i] == 3) low_score = 1000 * (i + 1); // score
    }

    // Two pairs, low_score = 1e2*max + 1e1*second
    int n_two_pairs = 0;
    for (int i = 8; i >= 0; i--) {
        if (value_counter[i] == 2) {
            n_two_pairs++;
            if (n_two_pairs == 1) {
                low_score += 100 * (i + 1);
            } else if (n_two_pairs == 2) {
                low_score += 10 * (i + 1);
            }
        }
    }

    // Full house, 1010 < score < 10000
    if (low_score >= 1100) { // 3+2
        return low_score; // return full house
    }

    if (score > 1000) return score; // return straight
    // return the rest exclude high card. Decay to 3-digits. 
    if (low_score > 0) return low_score / 10; 
    
    
    // High card
    int negative_score = - int_pow(10, 7);
    int power_counter = 0;
    for (int i = 0; i < 9; i++) {
        if (value_counter[i] == 1) {
            negative_score += i * int_pow(10,power_counter);
            power_counter ++;
        }
    }
    return negative_score;
}


void Holdem::get_probability (mt19937& gen) {
    array<int,8> rank_counter{};
    for (int match_i = 0; match_i < n_games; match_i++) { // each match
        init_card_pile();
        dealing_cards(gen);

        // Probability
        int my_score = calc_score(hole_cards[0][0], hole_cards[0][1]);
        int my_rank = 0;
        for (int i = 1; i < n_players; i++) {
            if (my_score < calc_score(hole_cards[i][0], hole_cards[i][1])) my_rank++;
        }
        rank_counter[my_rank] ++;
    }


    for (int i = 0; i < n_players; i++) {
        probability[i] = (double)rank_counter[i] / (double)n_games;
    }
}

void Holdem::test_game(mt19937& gen) {
        // TEST
        init_card_pile();
        dealing_cards(gen);
        cout << "community: ";
        // for (auto i : community_cards) {cout << i << " "; get_card_point(i); cout << endl; }
        for (auto i : community_cards) cout << get_card_point(i)[0] << get_card_point(i)[1] << " "; cout << endl;

        for (int i = 0; i < n_players; i++) {
            cout << "player" << i << ": ";
            // for (auto ii : hole_cards[i]) cout << ii << " "; cout << endl;
            for (auto ii : hole_cards[i]) cout << get_card_point(ii)[0] << get_card_point(ii)[1] << " ";
            cout << "Score: " << calc_score(hole_cards[i][0], hole_cards[i][1]);
            cout << endl;
        }
}


void worker_thread(
    int n_players, 
    int n_community_cards, 
    int* pknown_cards,
    double* rank_probabilities
) {
    // Random number generator for each thread. 
    random_device rd;
    mt19937 gen(rd());
    
    // Create a Holdem game
    Holdem gameHoldem;
    gameHoldem.n_players = n_players;
    gameHoldem.n_community_cards = n_community_cards;
    gameHoldem.pknown_cards = pknown_cards;
    gameHoldem.get_probability(gen);

    // Output results
    for (int i = 0; i < gameHoldem.n_players; i++) {
        rank_probabilities[i] += gameHoldem.probability[i];
    }
}

void test_thread (
    int n_players, 
    int n_community_cards, 
    int* pknown_cards
) {
    // Random number generator for each thread. 
    random_device rd;
    mt19937 gen(rd());
    
    // Create a Holdem game
    Holdem gameHoldem;
    gameHoldem.n_players = n_players;
    gameHoldem.n_community_cards = n_community_cards;
    gameHoldem.pknown_cards = pknown_cards;
    gameHoldem.test_game(gen);
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


    double total_prob[8];
    thread t1(worker_thread, n_people, n_community_cards, cards_arr.data(), total_prob);
    thread t2(worker_thread, n_people, n_community_cards, cards_arr.data(), total_prob);
    thread t3(worker_thread, n_people, n_community_cards, cards_arr.data(), total_prob);
    thread t4(worker_thread, n_people, n_community_cards, cards_arr.data(), total_prob);

    // thread t1(test_thread, n_people, n_community_cards, cards_arr.data());
    // thread t2(test_thread, n_people, n_community_cards, cards_arr.data());
    // thread t3(test_thread, n_people, n_community_cards, cards_arr.data());
    // thread t4(test_thread, n_people, n_community_cards, cards_arr.data());

    t1.join(); t2.join(); t3.join(); t4.join(); 

    for (int i = 0; i < n_people; i++) {
        double prob = total_prob[i] / 4.0 * 100.0;
        cout << format("{:.2f}%; ", prob);
    }
    cout << endl;

}

