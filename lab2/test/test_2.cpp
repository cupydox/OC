#include <gtest/gtest.h>
#include "../include/game.h"
#include <chrono>
#include <thread>
#include <numeric>


TEST(GameTests, TestDiceRoll) {
    int roll = rollDice();
    EXPECT_GE(roll, 2);
    EXPECT_LE(roll, 12);
}

TEST(GameTests, TestRunExperiment) {
    int wins1 = 0, wins2 = 0;
    runExperiment(10, 2, 10, 12, wins1, wins2);

    EXPECT_GE(wins1, 0);
    EXPECT_GE(wins2, 0);
    EXPECT_EQ(wins1 + wins2, 1);
}

TEST(GameTests, TestMultipleExperiments) {
    int wins1 = 0, wins2 = 0;
    const int experiments = 1000;
    for (int i = 0; i < experiments; ++i) {
        runExperiment(10, 2, 10, 12, wins1, wins2);
    }

    EXPECT_GT(wins1, 0);
    EXPECT_GT(wins2, 0);
    EXPECT_EQ(wins1 + wins2, experiments);
}

TEST(GameTests, TestWinProbability) {
    int wins1 = 0, wins2 = 0;
    const int experiments = 10000;
    for (int i = 0; i < experiments; ++i) {
        runExperiment(10, 0, 10, 10, wins1, wins2);
    }

    double prob1 = static_cast<double>(wins1) / experiments;
    double prob2 = static_cast<double>(wins2) / experiments;

    EXPECT_NEAR(prob1, 0.5, 0.05);
    EXPECT_NEAR(prob2, 0.5, 0.05);
}

TEST(GameTests, TestInitialAdvantage) {
    int wins1 = 0, wins2 = 0;
    const int experiments = 10000;
    for (int i = 0; i < experiments; ++i) {
        runExperiment(5, 0, 20, 10, wins1, wins2);
    }

    EXPECT_GT(wins1, wins2);
}


TEST(PerformanceTest, SingleVsMultiThread) {
    int iterations = 100000;
    int single_result = 0;
    int multi_result = 0;
    int num_threads = 4; 
    int wins_1 = 0, wins_2 = 0;

    auto start_single = std::chrono::high_resolution_clock::now();
    runExperiment(iterations, 0, 20, 10, wins_1, wins_2);
    auto end_single = std::chrono::high_resolution_clock::now();
    auto single_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_single - start_single).count();


    std::vector<std::thread> threads;
    std::vector<int> partial_wins1(num_threads, 0);
    std::vector<int> partial_wins2(num_threads, 0);

    int chunk_size = iterations / num_threads;

    auto start_multi = std::chrono::high_resolution_clock::now();

    int remaining = 100000; 
    for (int t = 0; t < num_threads; ++t) {
        int chunk_size = (remaining / (num_threads - t)); 
        remaining -= chunk_size; 

        threads.emplace_back([chunk_size, &partial_wins1, &partial_wins2, t]() {
            for (int i = 0; i < chunk_size; ++i) {
                int score1 = 0, score2 = 0;
                runExperiment(0, 1, score1, score2, partial_wins1[t], partial_wins2[t]);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int total_wins1 = std::accumulate(partial_wins1.begin(), partial_wins1.end(), 0);
    int total_wins2 = std::accumulate(partial_wins2.begin(), partial_wins2.end(), 0);

    auto end_multi = std::chrono::high_resolution_clock::now();
    auto multi_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_multi - start_multi).count();


}