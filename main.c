// 实验要求：
//  (1) 实现插入排序、归并排序、堆排序、快速排序算法;
// （2) 待排序数组规模 5~50万每隔5万取一组 （5万，10万，15万，20万，…，50万），
// 每种规模下随机产生10次，观察运行时间的均值和方差。
//我取的是0~0xfffffffe的随机数组
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "sys/time.h"
#define RANGE 0xffffffff //随机数范围
#define SIZE 50000       //随机数组增长大小
// #define TEST100          //是否使用100指定数据测试集，否则为循环数据输出
#define PRINT            //是否打印前100位
// #define MERGE_C99      //归并排序是否使用C99标准数组
#define ArrayLength(Array) (sizeof(Array) / sizeof(Array[0]))
/*不推荐在排序函数中包装此宏定义，原因是数组作为参数传给函数时，是传给数组的地址，
而不是传给整个的数组空间，因而sizeof(Array)这句话会报错*/

void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

//标准化数组输出
void print_array(int *array, int size)
{
    int *p = array;
    int *end = array + size;
    char *pad = "{ ";

    while (p < end)
    {
        printf("%s%d", pad, *p++);
        pad = ", ";
    }
    printf(" }\n");
}

//返回特定大小的int随机数组
int *rand_array(int arraysize)
{
    int *p = (int *)malloc(arraysize * sizeof(int));
    srand((unsigned)time(NULL));
    for (int i = 0; i < arraysize; i++)
    {
        *(p + i) = rand() % RANGE;
    }
    return p;
}

//Insertion Sort
void Insertion_sort(int A[], int length)
{
    int i, j;
    for (i = 1; i < length; i++)
    {
        j = i;
        while (j > 0 && A[j - 1] > A[j])
        {
            swap(&A[j], &A[j - 1]);
            j = j - 1;
        }
    }
}

//Merge Sort
void Merge(int A[], int l, int m, int r)
{
    //Array A and index l, m, r. given that A[l : m] and A[m + 1 : r] are sorted, return  Sorted array A[l : r]
    int p = m - l + 1;
    int q = r - m;
    int i, j, k;
#if defined(MERGE_C99)
    int L[p], R[q]; //辅助数组，C99标准更快，但由于是在栈上预分配，更容易爆栈
#else
    int *L = (int *)malloc(p * sizeof(int));
    int *R = (int *)malloc(q * sizeof(int)); //使用堆分配，速度慢些但是可以跑到更大的规模
#endif
    for (i = 0; i < p; i++)
    {
        L[i] = A[l + i];
    }
    for (j = 0; j < q; j++)
    {
        R[j] = A[m + 1 + j];
    }
    j = 0;
    i = 0;
    k = l;
    while (i < p && j < q)
    {
        if (L[i] <= R[j])
        {
            A[k] = L[i];
            i++;
        }
        else
        {
            A[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < p)
    {
        A[k] = L[i];
        i++;
        k++;
    }
    while (j < q)
    {
        A[k] = R[j];
        j++;
        k++;
    }
#if defined(MERGE_C99)
#else
    free(L);
    free(R);
#endif
}

void Merge_sort_partion(int A[], int begin, int end)
{
    int middle;
    if (begin < end)
    {
        middle = begin + (end - begin) / 2;
        Merge_sort_partion(A, begin, middle);
        Merge_sort_partion(A, middle + 1, end);
        Merge(A, begin, middle, end);
    }
}

void Merge_sort(int A[], int length)
{
    Merge_sort_partion(A, 0, length - 1);
}

//Heap Sort
void Heapify(int A[], int length, int i) //Max_Heapify 调整
{
    int largest = i;
    int l, r;
    l = 2 * i + 1;
    r = 2 * i + 2;
    if (l < length && A[l] > A[largest])
    {
        largest = l;
    }
    if (r < length && A[r] > A[largest])
    {
        largest = r;
    }
    if (largest != i)
    {
        swap(&A[i], &A[largest]);
        Heapify(A, length, largest);
    }
}

void Heap_sort(int A[], int length)
{
    int i;
    //初始化，i从最后一个父节点开始调整
    for (i = length / 2 - 1; i >= 0; i--)
    {
        Heapify(A, length, i);
    }
    //将第一个元素与排好序的元素组之前一个元素交换，再重新调整，直到排序完毕
    for (i = length - 1; i > 0; i--)
    {
        swap(&A[0], &A[i]);
        Heapify(A, i, 0);
    }
}

//Quick Sort
int Partition(int A[], int low, int high)
{
    int pivot = A[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++)
    {
        if (A[j] < pivot)
        {
            i++;
            swap(&A[i], &A[j]);
        }
    }
    swap(&A[i + 1], &A[high]);
    return i + 1;
}

void Quick_sort_inner(int A[], int low, int high)
{
    int pi;
    if (low < high)
    {
        pi = Partition(A, low, high);
        Quick_sort_inner(A, low, pi - 1);
        Quick_sort_inner(A, pi + 1, high);
    }
}

void Quick_sort(int A[], int length)
{
    Quick_sort_inner(A, 0, length - 1); //不优化开始输入的基准了
}

int main(int argc, char *argv[])
{
    struct timeval start;
    struct timeval end;
    unsigned long timer;
    int length;
#if defined(TEST100)
    int Test[100] = {77, 76, 61, 26, 10, 69, 37, 54, 2, 74, 
                     75, 6, 93, 97, 30, 48, 11, 44, 58, 37, 
                     87, 50, 13, 37, 97, 1, 1, 55, 12, 57, 
                     38, 73, 20, 2, 17, 55, 71, 76, 20, 76, 
                     48, 61, 10, 33, 92, 39, 8, 5, 79, 81, 
                     27, 15, 52, 85, 28, 97, 35, 62, 82, 95, 
                     17, 82, 33, 29, 2, 4, 87, 71, 56, 49, 
                     14, 32, 2, 47, 18, 15, 88, 71, 69, 74, 
                     97, 54, 90, 89, 15, 66, 16, 93, 20, 83, 
                     5, 24, 74, 69, 29, 77, 41, 99, 62, 37};
    length = ArrayLength(Test);

#if defined(PRINT)
    print_array(Test, 100); //排序前的前100位
#endif

    gettimeofday(&start, NULL);

    Insertion_sort(Test, length);
    // Merge_sort(Test, length);
    // Heap_sort(Test, length);
    // Quick_sort(Test, length);

    gettimeofday(&end, NULL);
#else
    for (int j = 1; j < 11; j++)
    {
        length = SIZE * j;
        printf("数据规模为%d\n", length);
        for (int i = 0; i < 10; i++)
        {

            int *Test = rand_array(SIZE * j);
            gettimeofday(&start, NULL);
            // Insertion_sort(Test, length);
            // Merge_sort(Test, length);
            // Heap_sort(Test, length);
            Quick_sort(Test, length);
            gettimeofday(&end, NULL);
#if defined(PRINT)
            print_array(Test, 100); //排序后的前100位
#endif
            timer = 1e6 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
            free(Test);
            printf("%ld\n", timer);
        }
    }
#endif
#if defined(TEST100)
#if defined(PRINT)
    print_array(Test, 100); //排序后的前100位
#endif
    timer = 1e6 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("排序花费时间为%ld微秒，也就是%f秒\n", timer, timer / 1e6);
    printf("%ld", timer);
#endif
    return 0;
}