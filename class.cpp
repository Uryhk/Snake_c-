#include "class.h"
#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <chrono>


using namespace std;

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

void showMenu() {
    clear();
    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "--- Juego de la Serpiente ---");
    mvprintw(LINES / 2, COLS / 2 - 5, "1. Jugar");
    mvprintw(LINES / 2 + 1, COLS / 2 - 5, "2. Puntuacion");
    mvprintw(LINES / 2 + 2, COLS / 2 - 5, "3. Salir");
    refresh();
}

void showScore() {
    clear();
    mvprintw(LINES / 2, COLS / 2 - 10, "Puntuacion mas alta: 100");
    mvprintw(LINES / 2 + 2, COLS / 2 - 15, "Presiona cualquier tecla para volver...");
    refresh();
    getch();
}
// Implementación de los métodos de la clase Snake
Snake::Snake(int startX, int startY) {
    head = new SnakeSegment{startX, startY, nullptr};
    tail = head;
    direction = 0;
    puntos = 0;
}

Snake::~Snake() {
    SnakeSegment* current = head;
    while (current != nullptr) {
        SnakeSegment* temp = current;
        current = current->next;
        delete temp;
    }
}

void Snake::move() {
    // 1. Crear un nuevo segmento en la nueva posición de la cabeza.
    SnakeSegment* newHead = new SnakeSegment;
    newHead->x = head->x;
    newHead->y = head->y;
    
    switch (direction) {
        case UP: newHead->y -= 1; break;
        case DOWN: newHead->y += 1; break;
        case LEFT: newHead->x -= 1; break;
        case RIGHT: newHead->x += 1; break;
    }

    // 2. Apuntar el nuevo segmento a la antigua cabeza
    newHead->next = head;
    head = newHead;
    
    // 3. Eliminar el segmento de la cola
    SnakeSegment* current = head;
    while (current->next != tail) {
        current = current->next;
    }
    delete tail;
    tail = current;
    tail->next = nullptr;
}

void Snake::grow() {
    // Lógica similar a move, pero sin eliminar la cola
    SnakeSegment* newHead = new SnakeSegment;
    newHead->x = head->x;
    newHead->y = head->y;

    switch (direction) {
        case UP: newHead->y -= 1; break;
        case DOWN: newHead->y += 1; break;
        case LEFT: newHead->x -= 1; break;
        case RIGHT: newHead->x += 1; break;
    }

    newHead->next = head;
    head = newHead;
    puntos++;
}

void Snake::changeDirection(int newDirection) {
    // Asegúrate de que no se pueda mover en la dirección opuesta
    if (abs(direction - newDirection) != 2) {
        direction = newDirection;
    }
}

bool Snake::checkWallCollision(int boardWidth, int boardHeight) {
    return (head->x <= 0 || head->x >= boardWidth - 1 ||
            head->y <= 0 || head->y >= boardHeight - 1);
}

bool Snake::checkSelfCollision() {
    SnakeSegment* current = head->next;
    while (current != nullptr) {
        if (head->x == current->x && head->y == current->y) {
            return true;
        }
        current = current->next;
    }
    return false;
}

SnakeSegment* Snake::getHead() const {
    return head;
}

int Snake::getPuntos() const {
    return puntos;
}

// Implementación de las funciones globales
void controles(Snake& snake) {
    int key = getch();       // lee tecla
    if (key == ERR) return;  // no hay tecla presionada

    switch (key) {
        case KEY_UP:    snake.changeDirection(UP); break;
        case KEY_DOWN:  snake.changeDirection(DOWN); break;
        case KEY_LEFT:  snake.changeDirection(LEFT); break;
        case KEY_RIGHT: snake.changeDirection(RIGHT); break;
        case 'x':      endwin(); exit(0); break; // salir
    }
}


void draw(char** board, Snake& snake, pair<int, int> food) {
    erase();
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            mvprintw(i, j * 2, "%c", board[i][j]);
        }
    }
    
    mvprintw(food.second, food.first * 2, "*");

    SnakeSegment* current = snake.getHead();
    while (current != nullptr) {
        mvprintw(current->y, current->x * 2, "o");
        current = current->next;
    }
    refresh();
}

pair<int, int> generateFood(char** board) {
    int x, y;
    do {
        x = rand() % (N - 2) + 1;
        y = rand() % (N - 2) + 1;
    } while (board[y][x] != ' ');
    return {x, y};
}

void run() {
    srand(time(0));
    bool gameOver = false;
    Snake mySnake(N / 2, N / 2);

    // Inicializar tablero
    char **board = new char*[N];
    for (int i = 0; i < N; i++) {
        board[i] = new char[N];
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            board[i][j] = (i==0 || i==N-1 || j==0 || j==N-1) ? '#' : ' ';
        }
    }

    pair<int,int> food = generateFood(board);

    // Configuración de ncurses para input rápido
    nodelay(stdscr, TRUE);   // getch() no bloquea
    keypad(stdscr, TRUE);    // detectar flechas
    curs_set(0);             // ocultar cursor

    int moveDelayMs = 150;          // cada cuánto mover la serpiente
    auto lastMove = std::chrono::steady_clock::now();

while (!gameOver) {
    int key = getch();           // revisa input siempre
    if (key != ERR) {
        switch(key) {
            case KEY_UP:    mySnake.changeDirection(UP); break;
            case KEY_DOWN:  mySnake.changeDirection(DOWN); break;
            case KEY_LEFT:  mySnake.changeDirection(LEFT); break;
            case KEY_RIGHT: mySnake.changeDirection(RIGHT); break;
            case 'x':      endwin(); exit(0); break;
        }
    }

    // mover la serpiente según tiempo transcurrido
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMove).count();
    if (elapsed >= moveDelayMs) {
        mySnake.move();
        lastMove = now;

        if (mySnake.checkWallCollision(N,N) || mySnake.checkSelfCollision())
            gameOver = true;

        if (mySnake.getHead()->x == food.first && mySnake.getHead()->y == food.second) {
            mySnake.grow();
            food = generateFood(board);
        }
    }

    draw(board, mySnake, food);  // dibujar siempre para input fluido
}


    // Limpiar memoria
    for (int i = 0; i < N; i++) delete[] board[i];
    delete[] board;

    // Mostrar Game Over
/**    clear();
    mvprintw(LINES/2, COLS/2 - 10, "Game Over!");
    mvprintw(LINES/2 + 1, COLS/2 - 15, "Puntuacion final: %d", mySnake.getPuntos());
    refresh();
    sleep(10);

    nodelay(stdscr, FALSE); // restaurar input bloqueante
}
**/    
    erase();
    mvprintw(LINES/2, COLS/2 - 10, "Game Over!");
    mvprintw(LINES/2 + 1, COLS/2 - 15, "Puntuacion final: %d", mySnake.getPuntos());
    mvprintw(LINES/2 + 3, COLS/2 - 20, "Presiona cualquier tecla para volver al menu...");
    refresh();
    nodelay(stdscr, FALSE); // restaurar input bloqueante
    getch();
}