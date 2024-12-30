#include <gtest/gtest.h>
#include <dlfcn.h>
#include <cmath>
#include <vector>
#include "../include/math_interface.h" 


template <typename FuncType>
FuncType LoadFunction(void* handle, const char* symbol) {
    FuncType func = reinterpret_cast<FuncType>(dlsym(handle, symbol));
    if (!func) {
        std::cerr << "Cannot load symbol " << symbol << ": " << dlerror() << std::endl;
    }
    return func;
}

TEST(StaticLinkingTest, E) {
    EXPECT_NEAR(E(10), 2.59374, 0.0001);
    EXPECT_NEAR(E(100), 2.70481, 0.0001);
}

TEST(StaticLinkingTest, BubbleSort) {
    int array[] = {5, 3, 8, 4, 2};
    int size = sizeof(array) / sizeof(array[0]);

    BubbleSort(array, size);

    EXPECT_EQ(array[0], 2);
    EXPECT_EQ(array[1], 3);
    EXPECT_EQ(array[2], 4);
    EXPECT_EQ(array[3], 5);
    EXPECT_EQ(array[4], 8);
}

TEST(DynamicLinkingTest, E) {
    void* handle = dlopen("./libmath2.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot open library: " << dlerror() << std::endl;
        return;
    }

    auto E = LoadFunction<float (*)(int)>(handle, "E");
    if (!E) {
        dlclose(handle);
        return;
    }

    EXPECT_NEAR(E(10), 2.71828, 0.0001);
    EXPECT_NEAR(E(100), 2.71828, 0.0001);

    dlclose(handle);
}

TEST(DynamicLinkingTest, QuickSort) {
    void* handle = dlopen("./libmath2.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot open library: " << dlerror() << std::endl;
        return;
    }

    auto QuickSort = LoadFunction<void (*)(int*, int, int)>(handle, "QuickSort");
    if (!QuickSort) {
        dlclose(handle);
        return;
    }

    int array[] = {5, 3, 8, 4, 2};
    int size = sizeof(array) / sizeof(array[0]);

    QuickSort(array, 0, size - 1);

    EXPECT_EQ(array[0], 2);
    EXPECT_EQ(array[1], 3);
    EXPECT_EQ(array[2], 4);
    EXPECT_EQ(array[3], 5);
    EXPECT_EQ(array[4], 8);

    dlclose(handle);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}