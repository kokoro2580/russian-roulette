#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

// ฟังก์ชันสร้างสำรับไพ่ Q, K, A, J
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

// ฟังก์ชันตรวจสอบจำนวนผู้เล่นที่ยังรอด
int countSurvivors(const vector<bool>& alive) {
    int count = 0;
    for (bool status : alive) {
        if (status) count++;
    }
    return count;
}

// ฟังก์ชันแสดงมือไพ่ของผู้เล่น
void displayHand(const vector<char>& hand) {
    for (char c : hand) {
        cout << c << " ";
    }
    cout << endl;
}

// ฟังก์ชันแจกไพ่ใหม่
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

    int currentPlayer = 0;
    bool gameRunning = true;
    vector<bool> alive(numPlayers, true);
    vector<char> tableCards = {'Q', 'K', 'A'};

    vector<char> discardedCards;
    int roundCount = 1;
    int bulletChance = 0;  // ตัวนับการหมุน Russian Roulette

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

        char tableCard = tableCards[rand() % tableCards.size()];
        cout << "\n=== Round " << roundCount << " ===" << endl;
        cout << "The table card is set. Players must claim to play: " << tableCard << endl;

        dealNewHands(hands, deck, numPlayers);

        string player = players[currentPlayer];
        cout << "\n" << player << "'s turn!" << endl;
        cout << "Your hand: ";
        displayHand(hands[currentPlayer]);

        int numCards;
        do {
            cout << "How many cards will you play (1-3)? ";
            cin >> numCards;
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
        for (char card : inputCards) {
            if (find(hands[currentPlayer].begin(), hands[currentPlayer].end(), card) == hands[currentPlayer].end()) {
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

        int nextPlayer = (currentPlayer + 1) % numPlayers;
        cout << players[nextPlayer] << ", do you challenge " << players[currentPlayer] << "'s play? (X to challenge, any key to continue): ";
        char challenge;
        cin >> challenge;

        if (challenge == 'X' || challenge == 'x') {
            cout << players[nextPlayer] << " is challenging " << players[currentPlayer] << "'s play!" << endl;

            bool lying = false;
            for (char c : inputCards) {
                if (c != tableCard && c != 'J') {
                    lying = true;
                    break;
                }
            }

            int victimIndex = lying ? currentPlayer : nextPlayer;
            cout << players[victimIndex] << " must take a bullet!" << endl;

            bulletChance = (bulletChance % 6) + 1;  // เพิ่มโอกาสหมุน Russian Roulette

            if (rand() % 6 == 0) {
                cout << "BANG! " << players[victimIndex] << " is eliminated!" << endl;
                alive[victimIndex] = false;
                bulletChance = 0;  // รีเซ็ตโอกาสหลังมีคนตาย
            } else {
                cout << "CLICK! No bullet. " << players[victimIndex] << " (" << bulletChance << "/6) survives." << endl;
            }

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
        }

        deck.insert(deck.end(), discardedCards.begin(), discardedCards.end());
        random_shuffle(deck.begin(), deck.end());
        discardedCards.clear();

        currentPlayer = (currentPlayer + 1) % numPlayers;
        roundCount++;
    }

    cout << "Game over! Thanks for playing Liar's Bar." << endl;
    return 0;
}
