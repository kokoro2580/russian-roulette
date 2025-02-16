#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sstream>
#include <limits>
#include <random>
using namespace std;

vector<char> createDeck();
int countSurvivors(const vector<bool>& alive);
void displayHand(const vector<char>& hand);
void dealNewHands(vector<vector<char>>& hands, vector<char>& deck, int numPlayers);
void gameLoop(vector<string>& players, int numPlayers);
void playTurn(int currentPlayer, vector<vector<char>>& hands, vector<char>& deck, vector<string>& players, vector<bool>& alive, vector<char>& tableCards, vector<char>& discardedCards, int& roundCount, bool& changeRound, vector<int>& shootCount);
bool challengePlayer(int currentPlayer, int nextPlayer, vector<char>& tableCards, vector<vector<char>>& hands, vector<string>& players, vector<bool>& alive, vector<char>& discardedCards, int& roundCount, vector<int>& shootCount);
void processChallenge(bool lying, int currentPlayer, int nextPlayer, vector<string>& players, vector<bool>& alive, int& roundCount, vector<int>& shootCount);
bool checkIfAllHandsEmpty(const vector<vector<char>>& hands);
void reshuffleDeck(vector<char>& deck, vector<char>& discardedCards);

int main() {
    srand(time(0));
    cout << "Welcome to Liar's Bar!" << endl;

    vector<string> players;
    int numPlayers;

    do {
        cout << "Enter number of players (2-4): ";
        cin >> numPlayers;
        cin.ignore();
    } while (numPlayers < 2 || numPlayers > 4);

    for (int i = 0; i < numPlayers; ++i) {
        string name;
        cout << "Enter name of player " << i + 1 << ": ";
        getline(cin, name);
        players.push_back(name);
    }

    gameLoop(players, numPlayers);

    cout << "Game over! Thanks for playing Liar's Bar." << endl;
    return 0;
}

void gameLoop(vector<string>& players, int numPlayers) {
    int currentPlayer = 0;
    bool gameRunning = true;
    vector<bool> alive(numPlayers, true);
    vector<char> tableCards{'Q', 'K', 'A'};
    vector<char> discardedCards;
    int roundCount = 1;
    vector<vector<char>> hands(numPlayers);
    vector<char> deck = createDeck();
    vector<int> shootCount(numPlayers, 0);  // เก็บจำนวนการยิงของผู้เล่นแต่ละคน

    cout << "\n=== Round " << roundCount << " ===" << endl;
    char tableCard = tableCards[rand() % tableCards.size()];
    cout << "The table card is set. Players must claim to play: " << tableCard << endl;

    while (gameRunning) {
        if (countSurvivors(alive) == 1) {
            for (int i = 0; i < numPlayers; ++i) {
                if (alive[i]) {
                    cout << players[i] << " is the winner!" << endl;
                    gameRunning = false;
                    break;
                }
            }
            break;
        }

        bool changeRound = false;
        while (!alive[currentPlayer]) {
            currentPlayer = (currentPlayer + 1) % numPlayers;
        }

        playTurn(currentPlayer, hands, deck, players, alive, tableCards, discardedCards, roundCount, changeRound, shootCount);

        reshuffleDeck(deck, discardedCards);

        if (changeRound) {
            roundCount++;
            if (roundCount > 1) {
                tableCard = tableCards[rand() % tableCards.size()];
                cout << "\n=== Round " << roundCount << " ===" << endl;
                cout << "The table card is set. Players must claim to play: " << tableCard << endl;
            }
            
            dealNewHands(hands, deck, numPlayers);
        }

        if (checkIfAllHandsEmpty(hands)) {
            cout << "\nAll hands are empty! Starting a new round..." << endl;
            tableCard = tableCards[rand() % tableCards.size()];
            dealNewHands(hands, deck, numPlayers);
            roundCount++;

            cout << "\n=== Round " << roundCount << " ===" << endl;
            cout << "The table card is set. Players must claim to play: " << tableCard << endl;
        }

        currentPlayer = (currentPlayer + 1) % numPlayers;
    }
}

void playTurn(int currentPlayer, vector<vector<char>>& hands, vector<char>& deck, vector<string>& players, vector<bool>& alive, vector<char>& tableCards, vector<char>& discardedCards, int& roundCount, bool& changeRound, vector<int>& shootCount) {
    if (hands[currentPlayer].empty()) {
        dealNewHands(hands, deck, players.size());
    }

    string player = players[currentPlayer];
    cout << "\n" << player << "'s turn!" << endl;
    cout << "Your hand: ";
    displayHand(hands[currentPlayer]);

    int numCards;
    do {
        cout << "How many cards will you play (1-3)? ";
        while (!(cin >> numCards) || numCards < 1 || numCards > 3) {
            cout << "Invalid input! Please enter a number between 1 and 3: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (numCards < 1 || numCards > 3);

    vector<char> inputCards;
    char cardInput;
    cout << "Enter the cards you claim to play (e.g., Q K A): ";
    cin.ignore();
    string input;
    getline(cin, input);
    stringstream ss(input);
    while (ss >> cardInput) {
        inputCards.push_back(cardInput);
    }

    bool valid = true;
    for (char card : inputCards) {
        if (find(hands[currentPlayer].begin(), hands[currentPlayer].end(), card) == hands[currentPlayer].end()) {
            valid = false;
            break;
        }
    }

    while (!valid || inputCards.size() != numCards) {
        cout << "You don't have those cards! Please enter valid cards (e.g., Q K A): ";
        getline(cin, input);
        stringstream ss(input);
        inputCards.clear();
        while (ss >> cardInput) {
            inputCards.push_back(cardInput);
        }

        valid = true;
        for (char card : inputCards) {
            if (find(hands[currentPlayer].begin(), hands[currentPlayer].end(), card) == hands[currentPlayer].end()) {
                valid = false;
                break;
            }
        }
    }

    for (char card : inputCards) {
        hands[currentPlayer].erase(find(hands[currentPlayer].begin(), hands[currentPlayer].end(), card));
        discardedCards.push_back(card);
    }

    int nextPlayer = (currentPlayer + 1) % players.size();
    while (!alive[nextPlayer]) {
        nextPlayer = (nextPlayer + 1) % players.size();
    }

    cout << players[nextPlayer] << ", do you challenge " << players[currentPlayer] << "'s play? (X to challenge, any key to continue): ";
    char challenge;
    cin >> challenge;
    cin.ignore();

    if (challenge == 'X' || challenge == 'x') {
        if (challengePlayer(currentPlayer, nextPlayer, tableCards, hands, players, alive, discardedCards, roundCount, shootCount)) {
            changeRound = true;
        }
    }
}

bool challengePlayer(int currentPlayer, int nextPlayer, vector<char>& tableCards, vector<vector<char>>& hands, vector<string>& players, vector<bool>& alive, vector<char>& discardedCards, int& roundCount, vector<int>& shootCount) {
    cout << players[nextPlayer] << " is challenging " << players[currentPlayer] << "'s play!" << endl;

    bool lying = false;
    for (char card : discardedCards) {
        if (card != tableCards[rand() % tableCards.size()] && card != 'J') {
            lying = true;
            break;
        }
    }

    processChallenge(lying, currentPlayer, nextPlayer, players, alive, roundCount, shootCount);
    return lying;
}

void processChallenge(bool lying, int currentPlayer, int nextPlayer, vector<string>& players, vector<bool>& alive, int& roundCount, vector<int>& shootCount) {
    int victimIndex = lying ? currentPlayer : nextPlayer;
    cout << players[victimIndex] << " must take a bullet!" << endl;

    int russianRoulette = rand() % 6 + 1;
    int chanceToShoot = 1 + shootCount[victimIndex];  // เพิ่มความเสี่ยงตามจำนวนการยิงของผู้เล่น

    cout << players[victimIndex] << " faces a " << chanceToShoot << "/6 chance to be shot!" << endl;

    if (russianRoulette <= chanceToShoot) {
        cout << "BANG! " << players[victimIndex] << " is eliminated!" << endl;
        alive[victimIndex] = false;
    } else {
        cout << "CLICK! No bullet. " << players[victimIndex] << " survives." << endl;
    }

    shootCount[victimIndex]++;  // เพิ่มจำนวนครั้งที่ยิงของผู้เล่นคนนั้น
}

bool checkIfAllHandsEmpty(const vector<vector<char>>& hands) {
    for (const auto& hand : hands) {
        if (!hand.empty()) {
            return false;
        }
    }
    return true;
}

void reshuffleDeck(vector<char>& deck, vector<char>& discardedCards) {
    deck.insert(deck.end(), discardedCards.begin(), discardedCards.end());
    random_shuffle(deck.begin(), deck.end());
    discardedCards.clear();
}

vector<char> createDeck() {
    vector<char> deck;
    for (int i = 0; i < 6; i++) {
        deck.push_back('Q');
        deck.push_back('K');
        deck.push_back('A');
    }
    deck.push_back('J');
    deck.push_back('J');
    random_shuffle(deck.begin(), deck.end());
    return deck;
}

int countSurvivors(const vector<bool>& alive) {
    int count = 0;
    for (bool status : alive) {
        if (status) count++;
    }
    return count;
}

void displayHand(const vector<char>& hand) {
    for (char card : hand) {
        cout << card << " ";
    }
    cout << endl;
}

void dealNewHands(vector<vector<char>>& hands, vector<char>& deck, int numPlayers) {
    shuffle(deck.begin(), deck.end(), default_random_engine(time(0)));
    
    for (int i = 0; i < numPlayers; ++i) {
        hands[i].clear(); 
        for (int j = 0; j < 5; ++j) { 
            hands[i].push_back(deck.back());
            deck.pop_back();
        }
    }
}
