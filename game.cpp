#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sstream>  // สำหรับแยก string เป็นหลายๆ คำ
using namespace std;

// ฟังก์ชันสร้างสำรับไพ่ Q, K, A, และ J
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

// ฟังก์ชันตรวจสอบว่าผู้เล่นมีไพ่ที่ต้องการลงหรือไม่
bool hasCard(const vector<char>& hand, char card) {
    return find(hand.begin(), hand.end(), card) != hand.end();
}

int main() {
    srand(time(0));  // ตั้งค่า seed สำหรับ rand
    cout << "Welcome to Liar's Bar!" << endl;
    
    vector<string> players;
    int numPlayers;
    
    // จำกัดจำนวนผู้เล่นระหว่าง 2-4 คน
    do {
        cout << "Enter number of players (2-4): ";
        cin >> numPlayers;
        cin.ignore();  // กำจัด newline character หลังจากการกรอกตัวเลข
    } while (numPlayers < 2 || numPlayers > 4);

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
    
    int currentPlayer = 0;
    bool gameRunning = true;
    vector<bool> alive(numPlayers, true);  // เช็คว่าผู้เล่นยังมีชีวิตอยู่หรือไม่
    vector<char> tableCards = {'Q', 'K', 'A'};  // ไพ่หัวโต๊ะที่เป็นไปได้

    while (gameRunning) {
        // ตรวจสอบว่ามีผู้เล่นเหลือรอดกี่คน
        if (countSurvivors(alive) == 1) {
            // หาผู้ชนะ
            for (int i = 0; i < numPlayers; ++i) {
                if (alive[i]) {
                    cout << players[i] << " is the winner!" << endl;
                    gameRunning = false;
                    break;
                }
            }
            break;
        }

        // ข้ามผู้เล่นที่ตายไปแล้ว
        while (!alive[currentPlayer]) {
            currentPlayer = (currentPlayer + 1) % numPlayers;
        }

        // ✅ **สุ่มไพ่หัวโต๊ะใหม่ทุกรอบ**
        char tableCard = tableCards[rand() % tableCards.size()];
        cout << "\n=== New Round ===" << endl;
        cout << "The table card is set. Players must claim to play: " << tableCard << endl;

        string player = players[currentPlayer];
        cout << "\n" << player << "'s turn!" << endl;
        cout << "Your hand: ";
        for (char c : hands[currentPlayer]) cout << c << " ";
        cout << endl;
        
        int numCards;
        do {
            cout << "How many cards will you play (1-3)? ";
            cin >> numCards;
        } while (numCards < 1 || numCards > 3);

        vector<char> playedCards;
        char cardInput;
        
        cout << "Enter the cards you claim to play (e.g., Q K A): ";
        
        // รับการกรอกหลายไพ่
        cin.ignore();  // เพื่อให้การกรอกในครั้งถัดไปทำได้ถูกต้อง
        string input;
        getline(cin, input);  // รับ input ทั้งหมด
        
        stringstream ss(input);  // ใช้ stringstream เพื่อแยก string
        vector<char> inputCards;
        while (ss >> cardInput) {
            inputCards.push_back(cardInput);
        }
        
        // ตรวจสอบว่าไพ่ที่กรอกมีในมือหรือไม่
        bool valid = true;
        for (char card : inputCards) {
            if (!hasCard(hands[currentPlayer], card)) {
                valid = false;
                break;
            }
        }

        // ถ้ากรอกไพ่ไม่ถูกต้อง, ถามใหม่
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
                if (!hasCard(hands[currentPlayer], card)) {
                    valid = false;
                    break;
                }
            }
        }
        
        // ลบไพ่ที่ผู้เล่นเลือกออกจากมือ
        for (char card : inputCards) {
            hands[currentPlayer].erase(find(hands[currentPlayer].begin(), hands[currentPlayer].end(), card));
        }

        // เช็คการท้าทาย
        char challenge;
        cout << "Next player, do you challenge? (X to challenge, any key to continue): ";
        cin >> challenge;
        
        if (challenge == 'X' || challenge == 'x') {
            bool lying = false;
            for (char c : inputCards) {
                if (c != tableCard && c != 'J') {
                    lying = true;
                    break;
                }
            }
            
            int victimIndex = lying ? currentPlayer : (currentPlayer + numPlayers - 1) % numPlayers;

            cout << players[victimIndex] << " must take a bullet!" << endl;

            // สุ่มว่ากระสุนนั้นจริงหรือเปล่า (1/6 มีจริง, 5/6 เปล่า)
            if (rand() % 6 == 0) {
                cout << "BANG! " << players[victimIndex] << " is eliminated!" << endl;
                alive[victimIndex] = false;
            } else {
                cout << "CLICK! No bullet. " << players[victimIndex] << " survives." << endl;
            }

            // เช็คว่าเหลือคนรอดเพียง 1 คนหรือยัง
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
        
        // วนไปยังผู้เล่นถัดไป
        currentPlayer = (currentPlayer + 1) % numPlayers;
    }
    
    cout << "Game over! Thanks for playing Liar's Bar." << endl;
    return 0;
}
    