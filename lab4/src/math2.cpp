#include <iostream>

extern "C" {
    float E(int x) {
        float e = 1.0;
        float factorial = 1.0;
        for (int n = 1; n <= x; ++n) {
            factorial *= n;
            e += 1.0 / factorial; 
        }
        return e;
    }

    void QuickSort(int *array, int low, int high) {
        if (low < high) {
            int pivot = array[high];
            int i = low - 1;

            for (int j = low; j < high; ++j) {
                if (array[j] < pivot) {
                    i++;
                    int temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }

            int temp = array[i + 1];
            array[i + 1] = array[high];
            array[high] = temp;

            int pi = i + 1;

            QuickSort(array, low, pi - 1);
            QuickSort(array, pi + 1, high);
        }
    }
}