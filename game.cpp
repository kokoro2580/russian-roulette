#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sstream>
#include <limits>
using namespace std;

vector<char> createDeck();
int countSurvivors(const vector<bool>& alive);
void displayHand(const vector<char>& hand);
void dealNewHands(vector<vector<char>>& hands, vector<char>& deck, int numPlayers);
void gameLoop(vector<string>& players, int numPlayers);
void playTurn(int currentPlayer, vector<vector<char>>& hands, vector<char>& deck, vector<string>& players, vector<bool>& alive, vector<char>& tableCards, vector<char>& discardedCards, int& roundCount);
bool challengePlayer(int currentPlayer, int nextPlayer, vector<char>& tableCards, vector<vector<char>>& hands, vector<string>& players, vector<bool>& alive, vector<char>& discardedCards, int& roundCount);
void processChallenge(bool lying, int currentPlayer, int nextPlayer, vector<string>& players, vector<bool>& alive, int& roundCount);
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

        while (!alive[currentPlayer]) {
            currentPlayer = (currentPlayer + 1) % numPlayers;
        }

        playTurn(currentPlayer, hands, deck, players, alive, tableCards, discardedCards, roundCount);

        reshuffleDeck(deck, discardedCards);

        currentPlayer = (currentPlayer + 1) % numPlayers;
    }
}

void playTurn(int currentPlayer, vector<vector<char>>& hands, vector<char>& deck, vector<string>& players, vector<bool>& alive, vector<char>& tableCards, vector<char>& discardedCards, int& roundCount) {
    // เริ่มต้นโดยไม่สุ่มไพ่หัวโต๊ะที่นี่
    char tableCard = tableCards[rand() % tableCards.size()];
    cout << "\n=== Round " << roundCount << " ===" << endl;
    cout << "The table card is set. Players must claim to play: " << tableCard << endl;

    dealNewHands(hands, deck, players.size());

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

    vector<char> playedCards;
    char cardInput;

    cout << "Enter the cards you claim to play (e.g., Q K A): ";
    cin.ignore();
    string input;
    getline(cin, input);

    stringstream ss(input);
    vector<char> inputCards;
    while (ss >> cardInput) {
        inputCards.push_back(cardInput);
    }

    bool valid = true;
    for (size_t i = 0; i < inputCards.size(); ++i) {
        if (find(hands[currentPlayer].begin(), hands[currentPlayer].end(), inputCards[i]) == hands[currentPlayer].end()) {
            valid = false;
            break;
        }
    }

    while (!valid || inputCards.size() != numCards) {
        cout << "You don't have that card! Please enter a valid card (e.g., Q K A): ";
        getline(cin, input);
        stringstream ss(input);
        inputCards.clear();
        while (ss >> cardInput) {
            inputCards.push_back(cardInput);
        }

        valid = true;
        for (size_t i = 0; i < inputCards.size(); ++i) {
            if (find(hands[currentPlayer].begin(), hands[currentPlayer].end(), inputCards[i]) == hands[currentPlayer].end()) {
                valid = false;
                break;
            }
        }
    }

    for (size_t i = 0; i < inputCards.size(); ++i) {
        hands[currentPlayer].erase(find(hands[currentPlayer].begin(), hands[currentPlayer].end(), inputCards[i]));
        discardedCards.push_back(inputCards[i]);
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
        if (challengePlayer(currentPlayer, nextPlayer, tableCards, hands, players, alive, discardedCards, roundCount)) {
            roundCount++;
            // ถ้ามีการท้าทาย สุ่มไพ่หัวโต๊ะใหม่
            tableCard = tableCards[rand() % tableCards.size()];
            cout << "New table card: " << tableCard << endl;
        }
    } else {
        if (checkIfAllHandsEmpty(hands)) {
            cout << "All players have run out of cards! The game continues to the next round." << endl;
            roundCount++;
        }
    }
}


bool challengePlayer(int currentPlayer, int nextPlayer, vector<char>& tableCards, vector<vector<char>>& hands, vector<string>& players, vector<bool>& alive, vector<char>& discardedCards, int& roundCount) {
    cout << players[nextPlayer] << " is challenging " << players[currentPlayer] << "'s play!" << endl;

    bool lying = false;
    for (size_t i = 0; i < discardedCards.size(); ++i) {
        if (discardedCards[i] != tableCards[rand() % tableCards.size()] && discardedCards[i] != 'J') {
            lying = true;
            break;
        }
    }

    processChallenge(lying, currentPlayer, nextPlayer, players, alive, roundCount);
    return lying;
}

void processChallenge(bool lying, int currentPlayer, int nextPlayer, vector<string>& players, vector<bool>& alive, int& roundCount) {
    int victimIndex = lying ? currentPlayer : nextPlayer;
    cout << players[victimIndex] << " must take a bullet!" << endl;

    int russianRoulette = rand() % 6 + 1;
    int chanceToShoot = roundCount;
    cout << players[victimIndex] << " faces a " << chanceToShoot << "/6 chance to be shot!" << endl;

    if (russianRoulette <= chanceToShoot) {
        cout << "BANG! " << players[victimIndex] << " is eliminated!" << endl;
        alive[victimIndex] = false;
    } else {
        cout << "CLICK! No bullet. " << players[victimIndex] << " survives." << endl;
    }
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
    for (size_t i = 0; i < alive.size(); ++i) {
        if (alive[i]) count++;
    }
    return count;
}

void displayHand(const vector<char>& hand) {
    for (size_t i = 0; i < hand.size(); ++i) {
        cout << hand[i] << " ";
    }
    cout << endl;
}

void dealNewHands(vector<vector<char>>& hands, vector<char>& deck, int numPlayers) {
    int deckIndex = 0;
    hands.clear();
    hands.resize(numPlayers);
    for (int i = 0; i < numPlayers; ++i) {
        for (int j = 0; j < 5; ++j) {
            hands[i].push_back(deck[deckIndex++]);
        }
    }
}
