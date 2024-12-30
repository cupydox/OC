#include "../include/game.h"
#include <cstdlib>
#include <pthread.h>

pthread_mutex_t mtx;

int rollDice() {
    return rand() % 6 + 1 + rand() % 6 + 1; 
}

void runExperiment(int k, int tour, int score1, int score2, int& wins1, int& wins2) {
    int player1_score = score1;
    int player2_score = score2;

    for (int j = tour + 1; j < k; ++j) {
        player1_score += rollDice();
        player2_score += rollDice();
    }

    pthread_mutex_lock(&mtx);
    if (player1_score > player2_score) {
        wins1++;
    } else {
        wins2++;
    }
    pthread_mutex_unlock(&mtx);
}

