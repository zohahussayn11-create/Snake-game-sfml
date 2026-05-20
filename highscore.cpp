#include "highscore.h"
#include <fstream>
using namespace std;

static const char* HS_FILE = "highscore.dat";

int loadHighScore() {
    ifstream f(HS_FILE);
    int hs = 0;
    if (f.is_open()) f >> hs;
    return hs;
}

void saveHighScore(int score) {
    int current = loadHighScore();
    if (score > current) {
        ofstream f(HS_FILE);
        if (f.is_open()) f << score;
    }
}
