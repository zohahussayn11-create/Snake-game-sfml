#pragma once
#include "common.h"
#include <vector>

void initSnake();
void movesnake(char direction);
void growsnake(char direction);
bool checkSelfCollision();
bool checkWallCollision(int w, int h);
std::vector<Point> getSnake();
