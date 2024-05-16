#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <cstring>

using std::cout;
using std::endl;

int partition(int *arr, int low, int high)
{
    int i = low;
    int j = high;
    int pivot = arr[(low + high) / 2]; // 取中间的数字作为基准值
    while (i <= j)
    {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j)
        {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }
    return i;
}

void serial_quicksort(int *arr, int low, int high)
{
    int new_low = partition(arr, low, high);
    int new_high = new_low - 1;
    if (low < new_high)
        serial_quicksort(arr, low, new_high);
    if (new_low < high)
        serial_quicksort(arr, new_low, high);
}

void parallel_quicksort(int *arr, int low, int high, int threshold = 1000)
{
    int new_low = partition(arr, low, high);
    int new_high = new_low - 1;
    // 未排序的数列长度小于阈值时，不采用并行
    if (high - low < threshold)
    {
        if (low < new_high)
            parallel_quicksort(arr, low, new_high);
        if (new_low < high)
            parallel_quicksort(arr, new_low, high);
    }
    else
    {
#pragma omp task untied
        {
            parallel_quicksort(arr, low, new_high);
        }
#pragma omp task untied
        {
            parallel_quicksort(arr, new_low, high);
        }
    }
}


int main(int argc, char *argv[])
{
    /*
        input: ./openMP_quicksort <len>
        (generate a sequence with a length of <len> randomly)
    */
    if (argc != 2)
    {
        cout << "Please input the length of the sequence" << endl;
        return 0;
    }

    // cout << "Original sequence: " << endl;
    int len = atoi(argv[1]);
    int *arr1 = new int[len];
    int *arr2 = new int[len];
    srand(time(0));
    for (int i = 0; i < len; i++)
    {
        arr1[i] = rand() % (len * 10);
        arr2[i] = arr1[i];
        // cout << arr1[i] << ", ";
    }
    // cout << endl;

    // Serial
    double start1 = omp_get_wtime();
    serial_quicksort(arr1, 0, len - 1);
    double end1 = omp_get_wtime();
    // cout << "Sorted sequence(Serial): " << endl;
    // for (int i = 0; i < len; i++)
    // {
    //     cout << arr1[i] << ", ";
    // }
    // cout << std::endl;
    cout << "Runtime(Serial)  : " << end1 - start1 << "s" << endl;

    // Parallel
    double start2 = omp_get_wtime();
#pragma omp parallel num_threads(4)
    {
#pragma omp single nowait
        {
            parallel_quicksort(arr2, 0, len - 1);
        }
    }
    double end2 = omp_get_wtime();
    // cout << "Sorted sequence(Parallel): " << endl;
    // for (int i = 0; i < len; i++)
    // {
    //     cout << arr2[i] << ", ";
    // }
    // cout << std::endl;
    cout << "Runtime(Parallel): " << end2 - start2 << "s" << endl;
    delete[] arr1;
    delete[] arr2;
}