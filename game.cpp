#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
using namespace std;

// สร้างสำรับไพ่ Q, K, A, และ J
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

int main() {
    srand(time(0));
    cout << "Welcome to Liar's Bar!" << endl;
    
    vector<string> players;
    int numPlayers;
    
    cout << "Enter number of players: ";
    cin >> numPlayers;
    cin.ignore();
    
    for (int i = 0; i < numPlayers; ++i) {
        string name;
        cout << "Enter name of player " << i + 1 << ": ";
        getline(cin, name);
        players.push_back(name);
    }
    
    // สร้างสำรับไพ่และแจกให้ผู้เล่น
    vector<vector<char>> hands(numPlayers);
    vector<char> deck = createDeck();
    int deckIndex = 0;
    for (int i = 0; i < numPlayers; ++i) {
        for (int j = 0; j < 5; ++j) {
            hands[i].push_back(deck[deckIndex++]);
        }
    }
    
    // กำหนดการ์ดบนโต๊ะ
    char tableCard = deck[deckIndex++];
    cout << "The table card is set. Players must claim to play: " << tableCard << endl;
    
    int currentPlayer = 0;
    bool gameRunning = true;
    vector<int> bullets(numPlayers, 0);
    
    while (gameRunning) {
        string player = players[currentPlayer];
        cout << "\n" << player << "'s turn!" << endl;
        cout << "Your hand: ";
        for (char c : hands[currentPlayer]) cout << c << " ";
        cout << endl;
        
        int numCards;
        cout << "How many cards will you play (1-3)? ";
        cin >> numCards;
        vector<char> playedCards(numCards);
        
        cout << "Enter the cards you claim to play (e.g., Q K A): ";
        for (int i = 0; i < numCards; ++i) {
            cin >> playedCards[i];
        }
        
        cout << "Next player, do you challenge? (X to challenge, any key to continue): ";
        char challenge;
        cin >> challenge;
        
        if (challenge == 'X' || challenge == 'x') {
            bool lying = false;
            for (char c : playedCards) {
                if (c != tableCard && c != 'J') {
                    lying = true;
                    break;
                }
            }
            
            if (lying) {
                cout << player << " was lying! They receive a bullet!" << endl;
                bullets[currentPlayer]++;
            } else {
                cout << "Honest play! Challenger receives a bullet!" << endl;
                bullets[(currentPlayer + numPlayers - 1) % numPlayers]++;
            }
        }
        
        if (bullets[currentPlayer] >= 1) {
            cout << player << " has lost the round!" << endl;
            gameRunning = false;
            break;
        }
        
        currentPlayer = (currentPlayer + 1) % numPlayers;
    }
    
    cout << "Game over! Thanks for playing Liar's Bar." << endl;
    return 0;
}
