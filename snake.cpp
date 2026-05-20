#include "snake.h"
#include <vector>
using namespace std;

static vector<Point> snake;

void initSnake() {
    snake.clear();
    snake.push_back({10, 10});
    snake.push_back({9,  10});
    snake.push_back({8,  10});
}

void movesnake(char direction) {
    Point newHead = snake[0];
    if (direction == 'R') newHead.a++;
    if (direction == 'L') newHead.a--;
    if (direction == 'U') newHead.b--;
    if (direction == 'D') newHead.b++;
    snake.insert(snake.begin(), newHead);
    snake.pop_back();
}

void growsnake(char direction) {
    Point newHead = snake[0];
    if (direction == 'R') newHead.a++;
    if (direction == 'L') newHead.a--;
    if (direction == 'U') newHead.b--;
    if (direction == 'D') newHead.b++;
    snake.insert(snake.begin(), newHead);
}

bool checkSelfCollision() {
    for (int i = 1; i < (int)snake.size(); i++)
        if (snake[0].a == snake[i].a && snake[0].b == snake[i].b)
            return true;
    return false;
}

bool checkWallCollision(int w, int h) {
    return snake[0].a < 0 || snake[0].a >= w ||
           snake[0].b < 0 || snake[0].b >= h;
}

vector<Point> getSnake() { return snake; }
