#include <iostream>
#include <ctime>
#include <vector>
#include <queue>

// length of array to be sorted
// note: the first element is located at index 1, rathen than 0
const int LENGTH = 2 << 20;

// return a random integer number in [start, end)
int getRandomNum(const int start, const int end);
void heapSort(const std::vector<int> constArray);
void siftDown(const int index, std::vector<int> &array, const int size);
void shellSort(const std::vector<int> constArray);
void mergeSort(const std::vector<int> constArray);
void merge(std::vector<int> &array, const int start, const int span);
void quickSort(const std::vector<int> constArray);
void subQuickSort(std::vector<int> &array, const int start, const int end);
void nonRecursiveQuickSortWithQueue(const std::vector<int> constArray);
void nonRecursiveQuickSortWithVector(const std::vector<int> constArray);

int main() {
    srand(time(NULL));

    std::vector<int> array = std::vector<int>(LENGTH + 1);
    for (int i = 1; i <= LENGTH; i++) {
        array[i] = getRandomNum(0, 10000);
    }

    heapSort(array);
    //shellSort(array);
    mergeSort(array);
    quickSort(array);
    nonRecursiveQuickSortWithQueue(array);
    nonRecursiveQuickSortWithVector(array);

    system("pause");
    return 0;
}

int getRandomNum(const int start, const int end) {
    int randomNum = (rand() % (end - start)) + start;
    return randomNum;
}

void heapSort(const std::vector<int> constArray) {
    std::vector<int> array = constArray;
    clock_t start = clock();
    
    // build heap
    int lastNonLeafNode = LENGTH / 2;
    for (int i = lastNonLeafNode; i >= 1; i--) {
        siftDown(i, array, LENGTH);
    }

    // sort
    std::vector<int> sortedArray;
    for (int i = 1; i <= LENGTH; i++) {
        const int size = LENGTH - i + 1;
        sortedArray.push_back(array[1]);
        array[1] = array[size];
        siftDown(1, array, size);
    }

    clock_t end = clock();
    std::cout << std::endl << "Time consumed: " << end - start << "ms" << std::endl;
}

void siftDown(const int index, std::vector<int> &array, const int size) {
    int cntIndex = index;
    while (cntIndex * 2 <= size) {
        const int left = cntIndex * 2;
        const int right = cntIndex * 2 + 1;
        if (left == size) {
            // has only left son
            if (array[cntIndex] > array[left]) {
                int tmp = array[left];
                array[left] = array[cntIndex];
                array[cntIndex] = tmp;
                cntIndex = left;
            }
            else
                break;
        }
        else {
            // has both left and right son
            if (array[left] <= array[right] && array[left] < array[cntIndex]) {
                int tmp = array[left];
                array[left] = array[cntIndex];
                array[cntIndex] = tmp;
                cntIndex = left;
            }
            else if (array[right] < array[left] && array[right] < array[cntIndex]) {
                int tmp = array[right];
                array[right] = array[cntIndex];
                array[cntIndex] = tmp;
                cntIndex = right;
            }
            else
                break;
        }
    }
}

void shellSort(const std::vector<int> constArray) {
    std::vector<int> array = constArray;
    clock_t start = clock();
    
    // determine step
    int step = 1;
    while (LENGTH >= step * 2) {
        step *= 2;
    }

    // sort
    while (step > 0) {
        for (int i = 1; i <= step; i++) {
            // sort in subarray
            for (int j = i; j <= LENGTH - step; j+=step) {
                for (int k = j + step; k <= LENGTH; k+=step) {
                    if (array[j] > array[k]) {
                        int tmp = array[j];
                        array[j] = array[k];
                        array[k] = tmp;
                    }
                }
            }
        }
        step /= 2;
    }

    clock_t end = clock();
    std::cout << std::endl << "Time consumed: " << end - start << "ms" << std::endl;
}

void mergeSort(const std::vector<int> constArray) {
    std::vector<int> array = constArray;
    clock_t start = clock();

    int span = 1;  // the length of subarray
    while (span < LENGTH) {
        for (int i = 1; i <= LENGTH; i+=(span * 2)) {
            merge(array, i, span);
        }
        span *= 2;
    }
    
    clock_t end = clock();
    std::cout << std::endl << "Time consumed: " << end - start << "ms" << std::endl;
}

void merge(std::vector<int> &array, const int start, const int span) {
    if (start + span - 1 >= LENGTH) {
        // don't have right subarray, do nothing
        return;
    }

    std::vector<int> tmpArray;
    int left = start;
    int right = start + span;
    while ((right < start + span * 2 && right <= LENGTH) || left < start + span) {
        if (left < start + span && ((right == start + span * 2 || right > LENGTH) || array[left] < array[right])) {
            tmpArray.push_back(array[left]);
            left++;
        }
        else {
            tmpArray.push_back(array[right]);
            right++;
        }
    }

    int end = start + span * 2 - 1;
    end = end <= LENGTH ? end : LENGTH;
    for (int i = start; i <= end; i++) {
        array[i] = tmpArray[i - start];
    }
}

void quickSort(const std::vector<int> constArray) {
    std::vector<int> array = constArray;
    clock_t start = clock();

    subQuickSort(array, 1, LENGTH);

    clock_t end = clock();
    std::cout << std::endl << "Time consumed: " << end - start << "ms" << std::endl;
}

void subQuickSort(std::vector<int> &array, const int start, const int end) {
    if (start >= end)
        return;
    array[0] = array[start];
    int left = start;
    int right = end;
    while (left < right) {
        while (left < right && array[right] >= array[0]) {
            right--;
        }
        array[left] = array[right];
        while (left < right && array[left] <= array[0]) {
            left++;
        }
        array[right] = array[left];
    }
    array[left] = array[0];
    subQuickSort(array, start, left - 1);
    subQuickSort(array, right + 1, end);
}

void nonRecursiveQuickSortWithQueue(const std::vector<int> constArray) {
    std::vector<int> array = constArray;
    clock_t start = clock();

    std::queue<std::pair<int, int>> q;
    q.push(std::pair<int, int>(1, LENGTH));

    while (!q.empty())
    {
        std::pair<int, int> cnt = q.front();
        q.pop();
        if (cnt.first >= cnt.second)
            continue;
        array[0] = array[cnt.first];
        int left = cnt.first;
        int right = cnt.second;
        while (left < right) {
            while (left < right && array[right] >= array[0]) {
                right--;
            }
            array[left] = array[right];
            while (left < right && array[left] <= array[0]) {
                left++;
            }
            array[right] = array[left];
        }
        array[left] = array[0];
        q.push(std::pair<int, int>(cnt.first, left - 1));
        q.push(std::pair<int, int>(right + 1, cnt.second));
    }

    clock_t end = clock();
    std::cout << std::endl << "Time consumed: " << end - start << "ms" << std::endl;
}

void nonRecursiveQuickSortWithVector(const std::vector<int> constArray) {
    std::vector<int> array = constArray;
    clock_t start = clock();

    std::vector<std::pair<int, int>> q = std::vector<std::pair<int, int>>(2<<20);
    q.push_back(std::pair<int, int>(1, LENGTH));

    while (!q.empty())
    {
        std::pair<int, int> cnt = q.front();
        q.pop_back();
        if (cnt.first >= cnt.second)
            continue;
        array[0] = array[cnt.first];
        int left = cnt.first;
        int right = cnt.second;
        while (left < right) {
            while (left < right && array[right] >= array[0]) {
                right--;
            }
            array[left] = array[right];
            while (left < right && array[left] <= array[0]) {
                left++;
            }
            array[right] = array[left];
        }
        array[left] = array[0];
        q.push_back(std::pair<int, int>(cnt.first, left - 1));
        q.push_back(std::pair<int, int>(right + 1, cnt.second));
    }

    clock_t end = clock();
    std::cout << std::endl << "Time consumed: " << end - start << "ms" << std::endl;
}