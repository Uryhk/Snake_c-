#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <utility>
#include "class.h" // Aquí se incluyen las declaraciones de todas tus clases y funciones

using namespace std;

int main() {
    // Configuración inicial de la terminal con ncurses
   initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);  // importante para flechas
    nodelay(stdscr, TRUE); // input no bloqueante
    timeout(-1);   // Espera indefinidamente por la entrada del usuario

    int choice;
    bool running = true;

    while (running) {
        showMenu(); // Muestra el menú principal
        choice = getch(); // Espera la entrada del usuario3

        switch (choice) {
            case '1':
                run(); // Llama a la función que inicia y corre el juego3
                break;
            case '2':
                showScore(); // Muestra la puntuación
                break;
            case '3':
                running = false; // Sale del bucle para terminar el programa
                break;
        }
    }

    // Limpia y restaura la terminal antes de salir
    endwin();

    return 0;
}