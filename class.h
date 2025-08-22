#ifndef SNAKE_H
#define SNAKE_H
#include <utility>

const int N = 20;
// Estructura para cada segmento del cuerpo
struct SnakeSegment {
    int x;
    int y;
    SnakeSegment* next;
};

class Snake {
private:
    SnakeSegment* head;
    SnakeSegment* tail;
    int direction;
    int puntos;

public:
    // Constructor
       Snake(int startX, int startY);
    ~Snake();
    void move();
    void grow();
    void changeDirection(int newDirection);
    bool checkWallCollision(int boardWidth, int boardHeight);
    bool checkSelfCollision();
    SnakeSegment* getHead() const;
    int getPuntos() const;
};

// Prototipos de funciones globales
void controles(Snake& snake);
void draw(char** board, Snake& snake, std::pair<int, int> food);
std::pair<int, int> generateFood(char** board);
void run();
void showMenu();
void showScore();

#endif // SNAKE_H