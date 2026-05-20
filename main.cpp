#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "common.h"
#include "snake.h"
#include "food.h"
#include "highscore.h"

// ── Palette ─────────────────────────────────────────────────────────────────
const sf::Color BG_DARK   (13,  17,  23);
const sf::Color GRID_LINE (25,  32,  40);
const sf::Color SNAKE_HEAD(80,  220, 120);
const sf::Color SNAKE_BODY(45,  160,  80);
const sf::Color SNAKE_TAIL(30,  110,  55);
const sf::Color FOOD_COLOR(255, 80,   80);
const sf::Color FOOD_GLOW (255, 150, 150);
const sf::Color PANEL_BG  (18,  24,  32);
const sf::Color TEXT_MAIN (220, 230, 240);
const sf::Color TEXT_DIM  (100, 120, 140);
const sf::Color ACCENT    (80,  220, 120);
const sf::Color DANGER    (255, 80,   80);

// ── Game state ───────────────────────────────────────────────────────────────
enum class Screen { MENU, PLAYING, GAME_OVER };

static Screen      screen      = Screen::MENU;
static char        direction   = 'R';
static char        nextDir     = 'R';
static bool        paused      = false;
static int         highScore   = 0;
static float       stepTimer   = 0.f;
static float       stepDelay   = 0.15f;   // seconds per tick
static float       foodPulse   = 0.f;

// ── Helpers ──────────────────────────────────────────────────────────────────
std::string intToStr(int v) {
    std::ostringstream ss; ss << v; return ss.str();
}

void centreText(sf::Text& t, float cx, float cy) {
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(cx, cy);
}

// Draw a rounded rectangle (approximated with a rectangle + circles at corners)
void drawRoundRect(sf::RenderWindow& win, float x, float y,
                   float w, float h, float r, sf::Color col) {
    sf::RectangleShape hRect(sf::Vector2f(w - 2*r, h));
    hRect.setPosition(x + r, y);
    hRect.setFillColor(col);
    win.draw(hRect);

    sf::RectangleShape vRect(sf::Vector2f(w, h - 2*r));
    vRect.setPosition(x, y + r);
    vRect.setFillColor(col);
    win.draw(vRect);

    sf::CircleShape corner(r);
    corner.setFillColor(col);
    auto drawCorner = [&](float cx, float cy) {
        corner.setPosition(cx - r, cy - r);
        win.draw(corner);
    };
    drawCorner(x + r,     y + r);
    drawCorner(x + w - r, y + r);
    drawCorner(x + r,     y + h - r);
    drawCorner(x + w - r, y + h - r);
}

// ── Drawing screens ──────────────────────────────────────────────────────────
void drawGrid(sf::RenderWindow& win) {
    sf::RectangleShape line;
    line.setFillColor(GRID_LINE);

    // Vertical lines
    for (int x = 0; x <= COLS; x++) {
        line.setSize({1.f, (float)(ROWS * CELL_SIZE)});
        line.setPosition((float)(x * CELL_SIZE), 0.f);
        win.draw(line);
    }
    // Horizontal lines
    for (int y = 0; y <= ROWS; y++) {
        line.setSize({(float)(COLS * CELL_SIZE), 1.f});
        line.setPosition(0.f, (float)(y * CELL_SIZE));
        win.draw(line);
    }
}

void drawSnake(sf::RenderWindow& win) {
    auto snake = getSnake();
    int  n     = (int)snake.size();

    for (int i = n - 1; i >= 0; i--) {
        float t = (n > 1) ? (float)(n - 1 - i) / (float)(n - 1) : 0.f;

        // Interpolate colour head→tail
        sf::Color c;
        c.r = (sf::Uint8)(SNAKE_HEAD.r + t * (SNAKE_TAIL.r - SNAKE_HEAD.r));
        c.g = (sf::Uint8)(SNAKE_HEAD.g + t * (SNAKE_TAIL.g - SNAKE_HEAD.g));
        c.b = (sf::Uint8)(SNAKE_HEAD.b + t * (SNAKE_TAIL.b - SNAKE_HEAD.b));
        c.a = 255;

        float margin = (i == 0) ? 1.f : 3.f;
        float sz     = CELL_SIZE - margin * 2;

        drawRoundRect(win,
            snake[i].a * CELL_SIZE + margin,
            snake[i].b * CELL_SIZE + margin,
            sz, sz,
            (i == 0) ? 5.f : 4.f,
            c);

        // Eyes on head
        if (i == 0) {
            sf::CircleShape eye(2.f);
            eye.setFillColor(sf::Color::Black);
            float hx = snake[0].a * CELL_SIZE + CELL_SIZE / 2.f;
            float hy = snake[0].b * CELL_SIZE + CELL_SIZE / 2.f;
            if (direction == 'R') { eye.setPosition(hx + 4, hy - 4); win.draw(eye); eye.setPosition(hx + 4, hy + 2); win.draw(eye); }
            if (direction == 'L') { eye.setPosition(hx - 8, hy - 4); win.draw(eye); eye.setPosition(hx - 8, hy + 2); win.draw(eye); }
            if (direction == 'U') { eye.setPosition(hx - 4, hy - 8); win.draw(eye); eye.setPosition(hx + 2, hy - 8); win.draw(eye); }
            if (direction == 'D') { eye.setPosition(hx - 4, hy + 4); win.draw(eye); eye.setPosition(hx + 2, hy + 4); win.draw(eye); }
        }
    }
}

void drawFood(sf::RenderWindow& win) {
    float pulse  = 0.5f + 0.5f * std::sin(foodPulse);
    float margin = 3.f - pulse * 2.f;
    float sz     = CELL_SIZE - margin * 2;

    // Glow
    sf::CircleShape glow(CELL_SIZE * 0.7f);
    glow.setFillColor(sf::Color(255, 80, 80, (sf::Uint8)(40 + 30 * pulse)));
    glow.setOrigin(CELL_SIZE * 0.7f, CELL_SIZE * 0.7f);
    glow.setPosition(getFoodX() * CELL_SIZE + CELL_SIZE / 2.f,
                     getFoodY() * CELL_SIZE + CELL_SIZE / 2.f);
    win.draw(glow);

    drawRoundRect(win,
        getFoodX() * CELL_SIZE + margin,
        getFoodY() * CELL_SIZE + margin,
        sz, sz, 5.f, FOOD_COLOR);
}

void drawHUD(sf::RenderWindow& win, const sf::Font& font) {
    // Panel background
    sf::RectangleShape panel(sf::Vector2f((float)WINDOW_W, (float)PANEL_HEIGHT));
    panel.setPosition(0.f, (float)(ROWS * CELL_SIZE));
    panel.setFillColor(PANEL_BG);
    win.draw(panel);

    // Divider line
    sf::RectangleShape div(sf::Vector2f((float)WINDOW_W, 2.f));
    div.setPosition(0.f, (float)(ROWS * CELL_SIZE));
    div.setFillColor(ACCENT);
    win.draw(div);

    float py = ROWS * CELL_SIZE + PANEL_HEIGHT / 2.f;

    // Score
    sf::Text lbl("SCORE", font, 13);
    lbl.setFillColor(TEXT_DIM);
    lbl.setLetterSpacing(2.f);
    centreText(lbl, WINDOW_W * 0.25f, py - 14.f);
    win.draw(lbl);

    sf::Text scoreT(intToStr(score), font, 26);
    scoreT.setFillColor(ACCENT);
    centreText(scoreT, WINDOW_W * 0.25f, py + 12.f);
    win.draw(scoreT);

    // Separator
    sf::RectangleShape sep(sf::Vector2f(2.f, 40.f));
    sep.setFillColor(GRID_LINE);
    sep.setPosition(WINDOW_W / 2.f, py - 20.f);
    win.draw(sep);

    // High score
    sf::Text hlbl("BEST", font, 13);
    hlbl.setFillColor(TEXT_DIM);
    hlbl.setLetterSpacing(2.f);
    centreText(hlbl, WINDOW_W * 0.75f, py - 14.f);
    win.draw(hlbl);

    sf::Text hsT(intToStr(highScore), font, 26);
    hsT.setFillColor(TEXT_MAIN);
    centreText(hsT, WINDOW_W * 0.75f, py + 12.f);
    win.draw(hsT);

    // Paused indicator
    if (paused) {
        sf::Text pt("⏸  PAUSED", font, 16);
        pt.setFillColor(DANGER);
        centreText(pt, WINDOW_W / 2.f, py);
        win.draw(pt);
    }
}

void drawMenu(sf::RenderWindow& win, const sf::Font& font) {
    // Animated gradient-ish background lines
    for (int i = 0; i < ROWS; i++) {
        sf::RectangleShape row(sf::Vector2f((float)WINDOW_W, (float)CELL_SIZE));
        row.setPosition(0.f, (float)(i * CELL_SIZE));
        sf::Uint8 val = (sf::Uint8)(13 + (i % 2) * 4);
        row.setFillColor(sf::Color(val, val + 4, val + 10));
        win.draw(row);
    }
    drawGrid(win);

    // Centre card
    float cx = WINDOW_W / 2.f;
    float cy = WINDOW_H / 2.f - 20.f;

    drawRoundRect(win, cx - 180, cy - 120, 360, 260, 16.f, sf::Color(22, 30, 42, 230));

    // Title
    sf::Text title("SNAKE", font, 72);
    title.setFillColor(ACCENT);
    centreText(title, cx, cy - 70.f);
    win.draw(title);

    sf::Text sub("RELOADED", font, 16);
    sub.setFillColor(TEXT_DIM);
    sub.setLetterSpacing(4.f);
    centreText(sub, cx, cy - 18.f);
    win.draw(sub);

    // Divider
    sf::RectangleShape div(sf::Vector2f(200.f, 2.f));
    div.setFillColor(ACCENT);
    div.setOrigin(100.f, 1.f);
    div.setPosition(cx, cy + 5.f);
    win.draw(div);

    sf::Text hint("Press  ENTER  to Play", font, 18);
    hint.setFillColor(TEXT_MAIN);
    centreText(hint, cx, cy + 40.f);
    win.draw(hint);

    sf::Text controls("WASD / Arrow Keys  ·  P to Pause", font, 13);
    controls.setFillColor(TEXT_DIM);
    centreText(controls, cx, cy + 75.f);
    win.draw(controls);

    if (highScore > 0) {
        sf::Text hs("Best: " + intToStr(highScore), font, 14);
        hs.setFillColor(FOOD_COLOR);
        centreText(hs, cx, cy + 105.f);
        win.draw(hs);
    }
}

void drawGameOver(sf::RenderWindow& win, const sf::Font& font) {
    // Dim the board
    sf::RectangleShape overlay(sf::Vector2f((float)WINDOW_W, (float)WINDOW_H));
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    win.draw(overlay);

    float cx = WINDOW_W / 2.f;
    float cy = WINDOW_H / 2.f - 30.f;

    drawRoundRect(win, cx - 190, cy - 110, 380, 240, 16.f, sf::Color(22, 30, 42, 245));

    sf::Text go("GAME OVER", font, 48);
    go.setFillColor(DANGER);
    centreText(go, cx, cy - 65.f);
    win.draw(go);

    sf::Text sc("Score: " + intToStr(score), font, 26);
    sc.setFillColor(TEXT_MAIN);
    centreText(sc, cx, cy - 5.f);
    win.draw(sc);

    bool newBest = (score >= highScore && score > 0);
    if (newBest) {
        sf::Text nb("🏆  NEW HIGH SCORE!", font, 18);
        nb.setFillColor(sf::Color(255, 200, 50));
        centreText(nb, cx, cy + 35.f);
        win.draw(nb);
    } else if (highScore > 0) {
        sf::Text hs("Best: " + intToStr(highScore), font, 16);
        hs.setFillColor(TEXT_DIM);
        centreText(hs, cx, cy + 35.f);
        win.draw(hs);
    }

    sf::Text r("ENTER — Play Again    ESC — Menu", font, 15);
    r.setFillColor(TEXT_DIM);
    centreText(r, cx, cy + 80.f);
    win.draw(r);
}

// ── Reset game ───────────────────────────────────────────────────────────────
void resetGame() {
    score     = 0;
    direction = 'R';
    nextDir   = 'R';
    stepTimer = 0.f;
    stepDelay = 0.15f;
    paused    = false;
    initSnake();
    generateFood();
}

// ── Main ─────────────────────────────────────────────────────────────────────
int main() {
    std::srand((unsigned)std::time(nullptr));
    highScore = loadHighScore();

    sf::RenderWindow window(
        sf::VideoMode(WINDOW_W, WINDOW_H),
        "Snake Reloaded",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    // Try to load a system font; fall back gracefully
    bool fontLoaded = font.loadFromFile("assets/font.ttf");
    if (!fontLoaded) {
        // Try common system paths
        const char* paths[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/consola.ttf",
            nullptr
        };
        for (int i = 0; paths[i] && !fontLoaded; i++)
            fontLoaded = font.loadFromFile(paths[i]);
    }

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        foodPulse += dt * 3.f;

        // ── Events ────────────────────────────────────────────────────────
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                auto k = event.key.code;

                if (screen == Screen::MENU) {
                    if (k == sf::Keyboard::Return) {
                        resetGame();
                        screen = Screen::PLAYING;
                    }
                }
                else if (screen == Screen::GAME_OVER) {
                    if (k == sf::Keyboard::Return) {
                        resetGame();
                        screen = Screen::PLAYING;
                    }
                    if (k == sf::Keyboard::Escape)
                        screen = Screen::MENU;
                }
                else if (screen == Screen::PLAYING) {
                    // Direction input (prevent 180°)
                    if ((k == sf::Keyboard::W || k == sf::Keyboard::Up)    && direction != 'D') nextDir = 'U';
                    if ((k == sf::Keyboard::S || k == sf::Keyboard::Down)  && direction != 'U') nextDir = 'D';
                    if ((k == sf::Keyboard::A || k == sf::Keyboard::Left)  && direction != 'R') nextDir = 'L';
                    if ((k == sf::Keyboard::D || k == sf::Keyboard::Right) && direction != 'L') nextDir = 'R';
                    if (k == sf::Keyboard::P) paused = !paused;
                    if (k == sf::Keyboard::Escape) screen = Screen::MENU;
                }
            }
        }

        // ── Update ────────────────────────────────────────────────────────
        if (screen == Screen::PLAYING && !paused) {
            stepTimer += dt;
            if (stepTimer >= stepDelay) {
                stepTimer -= stepDelay;
                direction = nextDir;

                movesnake(direction);

                if (checkFoodEaten()) {
                    growsnake(direction);
                    generateFood();
                    // Subtle speed-up every 50 pts
                    stepDelay = std::max(0.065f, stepDelay - 0.003f);
                    if (score > highScore) highScore = score;
                }

                if (checkSelfCollision() || checkWallCollision(COLS, ROWS)) {
                    saveHighScore(score);
                    highScore = loadHighScore();
                    screen = Screen::GAME_OVER;
                }
            }
        }

        // ── Render ────────────────────────────────────────────────────────
        window.clear(BG_DARK);

        if (screen == Screen::MENU) {
            drawMenu(window, font);
        }
        else {
            // Draw game area
            sf::RectangleShape arena(sf::Vector2f((float)WINDOW_W, (float)(ROWS * CELL_SIZE)));
            arena.setFillColor(BG_DARK);
            window.draw(arena);

            drawGrid(window);
            drawSnake(window);
            drawFood(window);
            drawHUD(window, font);

            if (screen == Screen::GAME_OVER)
                drawGameOver(window, font);
        }

        window.display();
    }

    return 0;
}
