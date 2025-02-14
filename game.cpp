#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sstream>
#include <limits>  // เพิ่มบรรทัดนี้
using namespace std;

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

void dealNewHands(vector<vector<char> >& hands, vector<char>& deck, int numPlayers) {
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
    vector<char> tableCards;
    tableCards.push_back('Q');
    tableCards.push_back('K');
    tableCards.push_back('A');

    vector<char> discardedCards;
    int roundCount = 1;

    vector<vector<char> > hands(numPlayers);
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

        // ข้ามผู้เล่นที่ถูกกำจัดไปยังผู้เล่นคนถัดไป
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

        int nextPlayer = (currentPlayer + 1) % numPlayers;
        
        // ข้ามผู้เล่นที่ถูกกำจัด
        while (!alive[nextPlayer]) {
            nextPlayer = (nextPlayer + 1) % numPlayers;
        }

        cout << players[nextPlayer] << ", do you challenge " << players[currentPlayer] << "'s play? (X to challenge, any key to continue): ";
        char challenge;
        cin >> challenge;
        cin.ignore();

        if (challenge == 'X' || challenge == 'x') {
            cout << players[nextPlayer] << " is challenging " << players[currentPlayer] << "'s play!" << endl;

            bool lying = false;
            for (size_t i = 0; i < inputCards.size(); ++i) {
                if (inputCards[i] != tableCard && inputCards[i] != 'J') {
                    lying = true;
                    break;
                }
            }

            int victimIndex = lying ? currentPlayer : nextPlayer;
            cout << players[victimIndex] << " must take a bullet!" << endl;

            // Russian Roulette: โอกาสการยิงจะเพิ่มขึ้นตามจำนวนรอบที่ท้าทาย
            int russianRoulette = rand() % 6 + 1;  // สุ่มระหว่าง 1 ถึง 6
            int chanceToShoot = roundCount;  // คำนวณความเสี่ยงจากรอบที่ท้าทาย

            // แสดงความเสี่ยงก่อนการยิง
            cout << players[victimIndex] << " faces a " << chanceToShoot << "/6 chance to be shot!" << endl;

            if (russianRoulette <= chanceToShoot) {
                cout << "BANG! " << players[victimIndex] << " is eliminated!" << endl;
                alive[victimIndex] = false;
            } else {
                cout << "CLICK! No bullet. " << players[victimIndex] << " survives." << endl;
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

            roundCount++;  // เพิ่มจำนวนรอบหลังจากการท้าทาย
        } else {
            bool allHandsEmpty = true;
            for (int i = 0; i < numPlayers; ++i) {
                if (!hands[i].empty()) {
                    allHandsEmpty = false;
                    break;
                }
            }

            if (allHandsEmpty) {
                cout << "All players have run out of cards! The game continues to the next round." << endl;
                roundCount++;
            }
        }

        deck.insert(deck.end(), discardedCards.begin(), discardedCards.end());
        random_shuffle(deck.begin(), deck.end());
        discardedCards.clear();

        currentPlayer = (currentPlayer + 1) % numPlayers;
    }

    cout << "Game over! Thanks for playing Liar's Bar." << endl;
    return 0;
}
