#include "food.h"
#include "snake.h"
#include <cstdlib>
#include <vector>
using namespace std;

int score = 0;

static int foodX, foodY;

void generateFood() {
    bool valid;
    do {
        valid = true;
        foodX = rand() % width;
        foodY = rand() % height;
        vector<Point> s = getSnake();
        for (auto& seg : s)
            if (seg.a == foodX && seg.b == foodY) { valid = false; break; }
    } while (!valid);
}

bool checkFoodEaten() {
    vector<Point> s = getSnake();
    if (s[0].a == foodX && s[0].b == foodY) {
        score += 10;
        return true;
    }
    return false;
}

int getFoodX()  { return foodX; }
int getFoodY()  { return foodY; }
int getScore()  { return score; }
