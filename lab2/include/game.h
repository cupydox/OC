#include <pthread.h>

extern pthread_mutex_t mtx;
int rollDice();

void runExperiment(int k, int tour, int score1, int score2, int& wins1, int& wins2);

struct ExperimentArgs {
    int k;
    int tour;
    int score1;
    int score2;
    int* wins1;
    int* wins2;
};

