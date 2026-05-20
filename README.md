🐍 Snake Reloaded
A modern remake of the classic Snake game, built in C++17 with the SFML 2.x graphics library. Smooth animations, progressive difficulty, and a persistent high-score system — all wrapped in a clean, modular architecture.
✨ Features

Color-gradient snake — segments transition from bright green (head) to dark green (tail)
Direction-aware eyes on the snake head that shift based on movement direction
Pulsing animated food with a sine-wave size effect and soft glow
Progressive difficulty — speed increases slightly with every food eaten
Persistent high score saved to disk (highscore.dat) and shown on both the menu and HUD
Pause / Resume with the P key
Three screen states — Main Menu, Gameplay, and Game Over with dark overlay
New high score trophy message on the Game Over screen
Delta-time movement for consistent speed across all machines


🎮 Controls
KeyActionW / ↑Move UpS / ↓Move DownA / ←Move LeftD / →Move RightPPause / ResumeEnterStart game / Play againEscapeReturn to Main Menu

🏗️ Project Architecture
The project follows a modular multi-file design — changes to one component don't affect the others.
Snake-Reloaded/
├── main.cpp              # Game loop, rendering, input, screen management
├── common.h              # Shared constants, grid size, Point struct
├── snake.h / snake.cpp   # Movement, growth, collision detection
├── food.h  / food.cpp    # Food placement, score tracking
├── highscore.h / highscore.cpp  # Save/load high score from disk
└── highscore.dat         # Persistent high score file (auto-generated)
Module Summary
ModuleFilesResponsibilityEntry Pointmain.cppGame loop, rendering, input, screen statesShared Headercommon.hConstants, grid size (30×24), Point structSnakesnake.h/cppMovement, growth, self & wall collisionFoodfood.h/cppRandom food placement, score (+10 per food)High Scorehighscore.h/cppFile I/O for persistent best score

⚙️ Game Mechanics
Movement — A double-buffer system ensures direction only changes at the start of a new tick, preventing input bugs. Pressing the opposite direction is ignored.
Collision Detection — Three types checked every tick:

Wall collision → head exits the 30×24 grid → Game Over
Self collision → head overlaps a body segment → Game Over
Food collision → head reaches food cell → Grow + Score

Scoring — Each food item awards 10 points and slightly reduces the movement tick interval. Minimum speed is capped to keep the game playable.

🛠️ Tech Stack
Tool / TechnologyPurposeC++17Core programming languageSFML 2.xWindow creation, rendering, input handlingg++ / MinGWCompilerVS CodeCode editorhighscore.datPersistent high score storage

🚀 Getting Started
Prerequisites

A C++17-compatible compiler (g++ / MinGW recommended)
SFML 2.x installed and linked

## Author

Developed by Zoha Hussayn
