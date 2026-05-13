#include <iostream>
#include <array>
#include <string>
#include <list>
#include <stdexcept>
#include <map>
#include <functional>
#include <random>
#include <thread>
#include <format>
#include <mutex>
#include <vector>
#include <cmath>
using namespace std;

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
/* 
usage: holdem_6p #players hole0 hole1 [community0 ~ community4] 
*/


// Basic Math
int int_pow(int base, int exponent) {
    if (exponent > 0) {
        exponent --;
        return base * int_pow(base, exponent);
    } else {
        return 1;
    }
}

double calculateMean(const vector<array<double,8>>& data, int column_index) {
    if (data.empty()) return 0.0;
    double sum = 0.0;
    for (auto i : data) {
        sum += i[column_index];
    }
    return sum / data.size();
}

double calculateStdev(const std::vector<array<double,8>>& data, int column_index, double mean) {
    if (data.size() <= 1) return 0.0;
    double varianceSum = 0.0;
    for (auto i : data) {
        varianceSum += pow(i[column_index] - mean, 2); // 每个数据与均值的平方差
    }
    return sqrt(varianceSum / data.size());
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
        long int calc_score(int hole0, int hole1);
        int n_games = 1000;
        array<double,8> probability;
        void get_probability(mt19937& gen);
        void test_game(mt19937& gen);

        Holdem(int n_players, int n_community_cards, int* pknown_cards) {
            this->n_players = n_players;
            this->n_community_cards = n_community_cards;
            this->pknown_cards = pknown_cards;
        }
};



void Holdem::init_card_pile() {
    for (int i = 0; i < 36; i++) {
        card_pile.push_back(i);
    }
}

void Holdem::dealing_cards(mt19937& gen) {
    // Take out from card pile
    for (int i = 0; i < n_community_cards + 2; i++) {
        int& card_i = pknown_cards[i];
        auto remove_counter = card_pile.remove(card_i);
        if (remove_counter != 1) {
            for (auto it : card_pile) cout << it << " "; cout << endl;
            cout << remove_counter << " of cards removed! -- " << card_i << endl;
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




long int Holdem::calc_score(int hole0, int hole1) {
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

    /* Methodology:
    ## max++: i.e. 6->1 (not 0), a->9 (not 8)
    1. four a kind (mutual exclusive) -> return score = (max4)E11 + (max1)E4
    2. straight -> score = (maxs)E8, straight flag
    3. flush -> score = (maxf)E10, flush flag
    4. if straight flag && flush flag -> check straight flush, return score = (maxsf)E12
        else return score (mutual exclusive with full house)
    5. three a kind -> score = (max3)E7, three flag
    6. pair -> score += (max2)E6 + (max2')E5, pair flag
    7. if three flag && pair flag -> return score *= 1E2 = (max3)E9 + (max2)E8

    Sort single cards at once. Instead of in each conditions.
    8. if three flag -> return score += (max1)E4 + (max1')E3
    9. if (pair flag > 1) -> return score += (max2')E5 + (max1)E4
    10. if (pair flag == 1) -> return score += (max1)E4 + (max1')E3 + (max1'')E2
    11. return score (max1)E4 + (max1')E3 + (max1'')E2 + (max1''')E1 + (max1'''')E0
    */

    long int score = 0;

    // 1. Four a kind, score = (max+1)E11
    // Mutual exclusive for all the rest cases. => Return score directly
    bool four_flag = false;
    for (int i = 9; i > 0; i--) {
        if (value_counter[i-1] == 4) {
            score = static_cast<long int>(1E11) * i;
            four_flag = true;
        }
    }
    if (four_flag) {
        for (int i = 9; i > 0; i--) {
            if (value_counter[i-1] && (value_counter[i-1] != 4)) return score + i * static_cast<long int>(1E4);
        }
    }

    // 2. Straight, score = (maxs)E8
    // method: count to 5
    bool straight_flag = false;
    int straight_counter = 0;
    int straight_max = 0;
    for (int i = 8; i >= 0; i--) { // high to low
        if (value_counter[i] > 0) {
            if (straight_counter == 0) straight_max = i + 1;
            straight_counter ++;
            if (straight_counter == 5) {
                score += static_cast<long>(1E8) * straight_max;
                straight_flag = true;
            }
        } else {
            straight_counter = 0;
            if (i < 4) break; // the only straight that < 10 is 9-A, which will be included below
        }
    }
    if (value_counter[8] > 0) { // if Ace exist, then check 9-A straight
        int i;
        for (i = 0; i < 4; i++) {
            if (value_counter[i] == 0) break;
        } 
        if (i == 3) {
            score = static_cast<long int>(4E8);
            straight_flag = true;
        }
    }

     // 3. Flush, score = (maxf)E10
    bool flush_flag = false;
    int flush_suit = 5; // which suit has flush, null = 5
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
        score = static_cast<long>(1E10) * (flush_max + 1);
        flush_flag = true;
    }

    // 4. straight flush, score = (maxsf)E12
    if (straight_flag && flush_flag) { // both straight and flush exist
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
                if (straight_counter == 4) { // straight flush achieved
                    return (card_max + 1) * static_cast<long int>(1E12);
                }
            } else {
                straight_counter = 0;
                card_max = *itr;
            }
            card_previous = *itr;
        }
    }

    // Return flush or straight (mutual exclusive with full house)
    if (score > 1) return score;

    // 5. Three a kind, score = (max3)E7
    int three_flag = 0;
    for (int i = 9; i > 0; i--) {
        if (value_counter[i - 1] == 3) {
            score = static_cast<long>(1E7) * i;
            three_flag = i;
            break;
        }
    }

    // 6. Pairs, score += (max2)E6 + (max2')E5
    int pairs_counter = 0;
    int additional_pair = 0;
    for (int i = 9; i > 0; i--) {
        if (value_counter[i - 1] >= 2 && i != three_flag) {
            pairs_counter++;
            if (pairs_counter == 1) {
                score += 1000000 * i;
            } else if (pairs_counter == 2) {
                score += 100000 * i;
            } else if (pairs_counter == 3) {
                additional_pair = i;
                break;
            };
        }
    }

    // 7. Full house, score = (max3)E9 + (max2)E8
    if (three_flag && pairs_counter) return score / static_cast<long int>(1E6) * static_cast<long int>(1E8);


    // Sort single cards
    int high_cards[5]; int high_cards_ptr = 0;
    for (int i = 9; i > 0; i--) {
        if (value_counter[i - 1] == 1) {
            high_cards[high_cards_ptr] = i;
            if (high_cards_ptr < 4) high_cards_ptr ++;
            else break;
        }
    }


    // 8. Three a kind, score = (max3)E7 + (max1)E4 + (max1')E3
    // definitely be 3+1+1, otherwise it is a bigger combination
    if (three_flag) return score + 10000 * high_cards[0] + 1000 * high_cards[1];

    // 9. Pairs, score = (max2)E6 + (max2')E5 + (max1)E4 + ....
    switch (pairs_counter) { // 2+2+1 or 2+1+1+1
        case 2: // 2+2+1
            return score + 10000 * high_cards[0];
        case 3: // 2+2+2
            return score + 10000 * ((high_cards[0] > additional_pair) ? high_cards[0] : additional_pair);
        case 1: // 2+1+1+1
            return score / 10 + 10000 * high_cards[0] + 1000 * high_cards[1] + 100 * high_cards[2];
        default:
            break;
    }

    // High card
    for (int i = 0; i < 5; i++) {
        score += int_pow(10, i) * high_cards[4 - i];
    }
    return score;
}


void Holdem::get_probability (mt19937& gen) {
    array<int,8> rank_counter{};
    for (int match_i = 0; match_i < n_games; match_i++) { // each match
        init_card_pile();
        dealing_cards(gen);

        // Probability
        long int my_score = calc_score(hole_cards[0][0], hole_cards[0][1]);
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




int n_threads = 10;
int thread_counter = 0;
vector<array<double,8>> results_collector;
mutex mtx;

void worker_thread(
    int n_players, 
    int n_community_cards, 
    int* pknown_cards
) {
    // Random number generator for each thread. 
    random_device rd;
    mt19937 gen(rd());
    
    // Create n_games Holdem games
    Holdem gameHoldem(n_players, n_community_cards, pknown_cards);
    gameHoldem.get_probability(gen);

    // Output results
    mtx.lock();
    // cout << thread_counter << ": ";
    thread_counter ++;
    for (int i = 0; i < gameHoldem.n_players; i++) {
        // cout << gameHoldem.probability[i] << " ";
        // rank_probabilities[i] += gameHoldem.probability[i];
        results_collector.emplace_back(gameHoldem.probability);
    }
    // cout << endl;
    mtx.unlock();
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
    Holdem gameHoldem(n_players, n_community_cards, pknown_cards);
    mtx.lock();
    gameHoldem.test_game(gen);
    mtx.unlock();
}


int main(int argc, char* argv[]) {
    array<int,7> cards_arr;
    bool test_mode = false;

    // Test Mode
    if (string arg_tmp = argv[1]; arg_tmp == "-T") {
        argc --;
        argv ++;
        test_mode = true;
    }


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


    // double total_prob[8];
    vector<thread> threads;

    if (test_mode) {
        for (int i = 0; i < n_threads; i++) {
            threads.emplace_back(test_thread, n_people, n_community_cards, cards_arr.data());
        }
        for (auto& i : threads) i.join();
    }

    else {
        for (int i = 0; i < n_threads; i++) {
            threads.emplace_back(worker_thread, n_people, n_community_cards, cards_arr.data());
        }
        for (auto& i : threads) i.join();

        for (int i = 0; i < n_people; i++) {
            // double prob = total_prob[i] / (double)n_threads * 100.0;
            double prob_mean = calculateMean(results_collector, i);
            double prob_stdev = calculateStdev(results_collector, i, prob_mean);
            cout << format("{:.2f}% ± {:.2f}%; ", prob_mean * 100.0, prob_stdev * 100.0);
        }
    cout << endl;
    }

}

