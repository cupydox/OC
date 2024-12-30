#include <iostream>
#include <vector>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include "include/game.h" 

struct ThreadData {
    int total_games;   
    int experiments;
    int tour;
    int score1;      
    int score2;        
    int local_wins1; 
    int local_wins2;  
};

void* threadExperiment(void* args) {
    ThreadData* data = static_cast<ThreadData*>(args);

    for (int i = 0; i < data->experiments; ++i) {
        runExperiment(data->total_games, data->tour, data->score1, data->score2, data->local_wins1, data->local_wins2);
    }

    return nullptr;
}

int main() {
    int k, total_games, experiments, tour, score1, score2;

    std::cout << "Введите количество потоков: ";
    std::cin >> k;
    std::cout << "Введите общее количество туров: ";
    std::cin >> total_games;
    std::cout << "Введите количество экспериментов: ";
    std::cin >> experiments;
    std::cout << "Введите текущий тур: ";
    std::cin >> tour;
    std::cout << "Введите начальный счёт игрока 1: ";
    std::cin >> score1;
    std::cout << "Введите начальный счёт игрока 2: ";
    std::cin >> score2;

    srand(time(0));

    int experiments_per_thread = experiments / k;
    int leftover_experiments = experiments % k;

    int total_wins1 = 0, total_wins2 = 0; 

    std::vector<pthread_t> threads(k);
    std::vector<ThreadData> thread_data(k);

    for (int i = 0; i < k; ++i) {
        int thread_experiments = experiments_per_thread + (i < leftover_experiments ? 1 : 0);

        thread_data[i] = {total_games, thread_experiments, tour, score1, score2, 0, 0};
        pthread_create(&threads[i], nullptr, threadExperiment, &thread_data[i]);
    }

    for (int i = 0; i < k; ++i) {
        pthread_join(threads[i], nullptr);
        total_wins1 += thread_data[i].local_wins1;
        total_wins2 += thread_data[i].local_wins2;
    }


    std::cout << "Total wins for Player 1: " << total_wins1 << std::endl;
    std::cout << "Total wins for Player 2: " << total_wins2 << std::endl;
    std::cout << "Player 1 chance: " << (static_cast<double>(total_wins1) / (total_wins1 + total_wins2)) * 100 << "%" << std::endl;
    std::cout << "Player 2 chance: " << (static_cast<double>(total_wins2) / (total_wins1 + total_wins2)) * 100 << "%" << std::endl;

    return 0;
}
