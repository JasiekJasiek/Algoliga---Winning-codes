#include <bits/stdc++.h>

using namespace std;

const int FOREST_SIZE = 30;
const int ALPHABET_SIZE = 27;

const int MAX_DEPTH = 6;
//const int MAX_REPLAY_STEPS = 2;

const int FORLOOP_COST = 3;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int licznik;

string PHRASE;

struct Move {
    int position;
    char letter;
};

struct Stats {
    int num_states_created = 0;
    int time_spent;
    int output_size = 0;

    void print() {
        cerr << "time_spent: " << time_spent << "ms" << endl;
    }
};

Stats stats;

int code(char letter) {
    if(letter >= 'A' && letter <= 'Z') {
        return letter - 'A' + 1;
    }
    return 0; // Spacja
}

int get_num_rolls_only_right(char from, char to) {
    if(code(from) <= code(to)) {
        return code(to) - code(from);
    }
    return ALPHABET_SIZE - code(from) + code(to);
}

int get_num_rolls(char from, char to) {
    return std::min({
        get_num_rolls_only_right(from, to),
        get_num_rolls_only_right(to, from),
        FORLOOP_COST + get_num_rolls_only_right(0, to),
        FORLOOP_COST + get_num_rolls_only_right(to, 0)
    });
}

std::string print_rolls(char from, char to) {
    int num_rolls = get_num_rolls(from, to);
    if (num_rolls == get_num_rolls_only_right(from, to)) {
        return std::string(num_rolls, '+');
    } else if (num_rolls == get_num_rolls_only_right(to, from)) {
        return std::string(num_rolls, '-');
    }
    std::string forloop = from < ALPHABET_SIZE/2 ? "[-]" : "[+]";
    if (num_rolls == FORLOOP_COST + get_num_rolls_only_right(0, to)) {
        return forloop + std::string(num_rolls - FORLOOP_COST, '+');
    }
    return forloop + std::string(num_rolls - FORLOOP_COST, '-');
}



int get_num_moves_only_right(int from, int to) {
    if(from <= to) {
        return to - from;
    }
    return FOREST_SIZE - from + to;
}

int get_num_moves(int from, int to) {
    return std::min(
        get_num_moves_only_right(from, to),
        get_num_moves_only_right(to, from)
    );
}

std::string print_moves(char from, char to) {
    int num_moves = get_num_moves(from, to);
    if (num_moves == get_num_moves_only_right(from, to)) {
        return std::string(num_moves, '>');
    }
    return std::string(num_moves, '<');
}

class State {
  public:
    State() {
        stones_ = std::string(FOREST_SIZE, ' ');
        initial_position_ = total_cost_ = num_spelled_ = 0;
    }

    void add_move(Move move) {
        total_cost_ += move_cost(move);
        moves_ += move.position;
        moves_ += move.letter;
        stones_[move.position] = move.letter;
        num_spelled_ += 1;
    }

    void reset_state() {
        initial_position_ = current_position();
        moves_.clear();
        total_cost_ = 0;
    }

    void set_num_spelled(int ns) {
        num_spelled_ = ns;
    }

    int num_spelled() const {
        return num_spelled_;
    }

    char current_position() const {
        if(moves_.empty()) {
            return initial_position_;
        }
        return moves_[moves_.size()-2];
    }

    char current_letter() const {
        return stone(current_position());
    }

    const std::string moves() const {
        return moves_;
    }

    char stone(int position) const {
        return stones_[position];
    }

    void change_stone(int position, char letter){
        stones_[ position ] = letter;
    }

    int move_cost(Move move) const {
        return get_num_moves(current_position(), move.position) +
               get_num_rolls(stones_[move.position], move.letter);
    }

    int total_cost() const {
        return total_cost_;
    }

  private:
    std::string stones_;
    std::string moves_;
    int initial_position_, total_cost_, num_spelled_;
};

std::string replay_moves(const State& initial_state, const std::string& moves) {
    std::string output;
    State state = initial_state;
    for(int i = 0; i < moves.size(); i += 2) {
        output += print_moves(state.current_position(), moves[i]);
        output += print_rolls(state.stone(moves[i]), moves[i+1]);
        //output += ".";
        state.add_move({moves[i], moves[i+1]});
        //licznik++;
    }
    return output;
}

State build_next_state(const State& state, Move move) {
    State new_state = state;
    new_state.add_move({move.position, move.letter});
    stats.num_states_created += 1;
    return new_state;
}

void get_best_state(const State& state, int max_num_moves, 
                    int cost_so_far, std::optional<State>& best_state) {
    if (max_num_moves == 0 || state.num_spelled() == PHRASE.size()) {
        if(!best_state.has_value() || state.total_cost() < best_state.value().total_cost()) {
            best_state = state;
        }
        return;
    }
    for(char new_position = 0; new_position < FOREST_SIZE; new_position++) {
        Move move = {new_position, PHRASE[state.num_spelled()]};
        int next_move_cost_so_far = cost_so_far + state.move_cost(move);
        if (best_state.has_value() && next_move_cost_so_far >= best_state.value().total_cost()) {
            continue;
        }
        get_best_state(build_next_state(state, move), 
                       max_num_moves - 1, 
                       next_move_cost_so_far,
                       best_state); 
    }
}

bool if_alphabet(const std::string& s){
    if(s.size() < ALPHABET.size()){
        return false;
    }
    for(int i = 0; i < ALPHABET.size(); ++i){
        if(s[ i ] != ALPHABET[ i ]){
            return false;
        }
    }
    return true;
}

std::pair<std::string, int> get_shortest_repetition(const std::string& s) {
    for(int len = 1; len < std::min(static_cast<int>(s.size()), FOREST_SIZE-1); len++) {
        int found_reps = 1;
        for(int rep = 2;; rep++) {
            if(rep > 26){
                break;
            }
            int i;
            for(i = 0; i < len; i++) {
                if(len*(rep-1)+i >= s.size() || s[i] != s[len*(rep-1)+i]) {
                    break;
                }
            }
            if(i == len) {
                found_reps = rep;
            } 
            else {
                break;
            }
        }
        if(found_reps > 1) {
            return {s.substr(0, len), found_reps};
        }
    }
    return {"", 0};
}

std::pair<std::string, State> apply_forloop_repetition(const State& initial_state, 
                               const std::string pattern,
                               int reps) {
    State state = initial_state;
    bool jest_spacja = false;
    bool pierwsza_spajca = false;
    bool one_element_pattern = false;
    licznik = 2;
    // tworze counter
    if(pattern.size() == 1){
        one_element_pattern = true;
    }
    state.add_move({state.current_position(), char('A' + reps - 1)});
    // tworze spacje obok
    if (!one_element_pattern){
        licznik++;
        state.add_move({(state.current_position() + 1) % FOREST_SIZE, ' '});
    }
    // wypisuje pattern
    
    if(pattern[ 0 ] == ' '){
        pierwsza_spajca = true;
    }
    for(int i = 0; i < pattern.size(); i++) {
        if(pattern[ i ] == ' ' && i != 0){
            jest_spacja = true;
        }
        state.add_move({(state.current_position() + 1) % FOREST_SIZE, pattern[i]});
        licznik++;
    }
    if (!one_element_pattern){
        licznik++;
        state.add_move({(state.current_position() + 1) % FOREST_SIZE, ' '});
    }
    state.add_move({initial_state.current_position(), char('A' + reps - 1)});
    std::string output = replay_moves(initial_state, state.moves());
    if(jest_spacja){
        if(pierwsza_spajca){
            output += "[>>[.>].>[.>]<<<[<]<<-]";
        }
        else{
            output += "[>>[.>].>[.>]<<<[<]<-]";
        }
    }
    else{
        if(pierwsza_spajca){
            output += "[>>.>[.>]<[<]<<-]";
        }
        else{
            if(one_element_pattern){
                output += "[>.<-]";
            }
            else{
                output += "[>>[.>]<[<]<-]";
            }
        }      
    } 
    state.add_move({initial_state.current_position(), ' '});
    state.set_num_spelled(state.num_spelled() + (pattern.size() * reps) - (licznik + 1));
    return {output, state};
}

bool alphabet_with_spaces(const std::string& s){
    int licznik_alfabet = 0;
    if(s.size() < ALPHABET.size()){
        return false;
    }
    for(int i = 0; i < ALPHABET.size(); ++i){
        if(s[ licznik_alfabet ] != ALPHABET[ i ]){
            return false;
        }
        licznik_alfabet += 2;
    }
    return true;
}

int main()
{
    getline(cin, PHRASE);

    State current_state;

    while(current_state.num_spelled() < PHRASE.size()) {
        std::optional<State> new_state;
        std::string output = "";
        std::string still_to_spell = PHRASE.substr(current_state.num_spelled());

        if(if_alphabet(still_to_spell)){
            if(current_state.current_position() == ' '){
                cout << "+[.+]";
            }
            else{
                cout << print_rolls(current_state.stone(current_state.current_position()), 'A');
                cout << "[.+]";
            }
            current_state.change_stone(current_state.current_position(), ' ');
            current_state.set_num_spelled(current_state.num_spelled() + ALPHABET.size());
            current_state.reset_state();
            //cerr << current_state.num_spelled() << '\n';
            continue;
        }
        else if(alphabet_with_spaces(still_to_spell)){
            cout << print_rolls(current_state.stone(current_state.current_position()), 'A') << ">";
            cout << print_rolls(current_state.stone((current_state.current_position() + 1 % FOREST_SIZE)), ' ') << "<";
            cout << ".+[>.<.+]";
            current_state.change_stone(current_state.current_position(), ' ');
            current_state.set_num_spelled((current_state.num_spelled() + ALPHABET.size() * 2) - 1);
            current_state.reset_state();
            //cerr << current_state.num_spelled() << '\n';
            continue;
        }
        auto [prefix, rep] = get_shortest_repetition(still_to_spell);
        if(rep > 5) {
            rep = std::min(rep, ALPHABET_SIZE);
            auto [output, new_state] = apply_forloop_repetition(
                current_state,
                prefix,
                rep
            );
            cout << output;
            stats.output_size += output.size();
            current_state = new_state;
            current_state.reset_state();
        } else {
            get_best_state(current_state, MAX_DEPTH, 0, new_state);
            Move move = {
                new_state.value().moves()[0], new_state.value().moves()[1]
            };
            std::string output_moves = print_moves(current_state.current_position(), new_state.value().moves()[0]);
            std::string output_rolls = print_rolls(current_state.stone(new_state.value().moves()[0]), new_state.value().moves()[1]);
            output = output_moves + output_rolls + ".";
            current_state.add_move(move);
            cout << output;
            stats.output_size += output.size();
            current_state.reset_state();
        }
        //cerr << current_state.num_spelled() << '\n';
    }
    cout << endl;

}
