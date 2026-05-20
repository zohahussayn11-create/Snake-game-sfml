#pragma once

const int CELL_SIZE = 24;
const int COLS = 30;
const int ROWS = 24;
const int PANEL_HEIGHT = 80;
const int WINDOW_W = COLS * CELL_SIZE;
const int WINDOW_H = ROWS * CELL_SIZE + PANEL_HEIGHT;

// Legacy aliases used by food/snake logic
const int width  = COLS;
const int height = ROWS;

struct Point { int a, b; };

extern int score;
