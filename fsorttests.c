/* Tests functions for sorting arrays in parallel.
 * CSC 357, Assignment 7
 * Given tests, Winter '24. */

#include <sys/resource.h>
#include <assert.h>
#include <stdio.h>
#include "fsort.h"

int intcmp(const void *a, const void *b) {
    return *((int *)a) - *((int *)b);
}

int chrcmp(const void *a, const void *b) {
    return *((char *)a) - *((char *)b);
}

/* test01: Tests sorting trivial arrays. */
void test01() {
    int arr[] = {1};

    assert(fsort(arr, 0, sizeof(int), 8, intcmp) == 0);
    assert(fsort(arr, 1, sizeof(int), 8, intcmp) == 0);

    assert(arr[0] == 1);
}

/* test02: Tests sorting with trivial processes== */
void test02() {
    int arr[] = {2, -1, 9, 8, 5, -3, 0, 8};

    assert(fsort(arr, 8, sizeof(int), 8, intcmp) == 0);

    assert(arr[0] == -3);
    assert(arr[1] == -1);
    assert(arr[2] == 0);
    assert(arr[3] == 2);
    assert(arr[4] == 5);
    assert(arr[5] == 8);
    assert(arr[6] == 8);
    assert(arr[7] == 9);
}

/* test03: Tests sorting integers. */
void test03() {
    int arr[] = {2, -1, 9, 8, 5, -3, 0, 8};

    assert(fsort(arr, 8, sizeof(int), 4, intcmp) == 0);

    assert(arr[0] == -3);
    assert(arr[1] == -1);
    assert(arr[2] == 0);
    assert(arr[3] == 2);
    assert(arr[4] == 5);
    assert(arr[5] == 8);
    assert(arr[6] == 8);
    assert(arr[7] == 9);

}

/* test04: Tests sorting characters. */
void test04() {
    char arr[] = {'a', 'c', 'b', 'a', 'b', 'c', 'a', 'c'};

    assert(fsort(arr, 8, sizeof(char), 4, chrcmp) == 0);

    assert(arr[0] == 'a');
    assert(arr[1] == 'a');
    assert(arr[2] == 'a');
    assert(arr[3] == 'b');
    assert(arr[4] == 'b');
    assert(arr[5] == 'c');
    assert(arr[6] == 'c');
    assert(arr[7] == 'c');

}

/* test05: Tests sorting with failing forks. */
void test05() {
    int arr[] = {2, -1, 9, 8, 5, -3, 0, 8};
    struct rlimit limit;

    getrlimit(RLIMIT_NPROC, &limit);
    limit.rlim_cur = 1;
    setrlimit(RLIMIT_NPROC, &limit);

    /* NOTE: With the soft limit on number of processes lowered to 1, the first
     *       call to fork will immediately fail... */
    assert(fsort(arr, 8, sizeof(int), 4, intcmp) == 1);

    assert(arr[0] == 2);
    assert(arr[1] == -1);
    assert(arr[2] == 9);
    assert(arr[3] == 8);
    assert(arr[4] == 5);
    assert(arr[5] == -3);
    assert(arr[6] == 0);
    assert(arr[7] == 8);

    /* NOTE: ...however, as long as no calls to fork are ever made, the sort
     *       should succeed. */
    assert(fsort(arr, 8, sizeof(int), 8, intcmp) == 0);

    assert(arr[0] == -3);
    assert(arr[1] == -1);
    assert(arr[2] == 0);
    assert(arr[3] == 2);
    assert(arr[4] == 5);
    assert(arr[5] == 8);
    assert(arr[6] == 8);
    assert(arr[7] == 9);

    getrlimit(RLIMIT_NPROC, &limit);
    limit.rlim_cur = limit.rlim_max;
    setrlimit(RLIMIT_NPROC, &limit);

}

void testEmptyArray() {
    int *arr = NULL;

    assert(fsort(arr, 0, sizeof(int), 4, intcmp) == 0);
    // Since the array is empty, there's nothing to check
    //printf("testEmptyArray works\n");
}

void testSingleElementArray() {
    int arr[] = {42};

    assert(fsort(arr, 1, sizeof(int), 4, intcmp) == 0);
    assert(arr[0] == 42);
    //printf("testSingleElementArray works\n");
}

void testSortedArray() {
    int arr[] = {-3, -1, 0, 2, 5, 8, 8, 9};

    assert(fsort(arr, 8, sizeof(int), 4, intcmp) == 0);
    assert(arr[0] == -3);
    assert(arr[1] == -1);
    assert(arr[2] == 0);
    assert(arr[3] == 2);
    assert(arr[4] == 5);
    assert(arr[5] == 8);
    assert(arr[6] == 8);
    assert(arr[7] == 9);
    //printf("testSortedArray works\n");
}

void testIdenticalElementsArray() {
    int arr[] = {7, 7, 7, 7, 7, 7, 7, 7};

    assert(fsort(arr, 8, sizeof(int), 4, intcmp) == 0);
    for (int i = 0; i < 8; i++) {
        assert(arr[i] == 7);
    }
    //printf("testIdenticalElementsArray works\n");
}

int shortcmp(const void *a, const void *b) {
    return *((short *)a) - *((short *)b);
}

void testShortArray() {
    short arr[] = {3, 1, 2, 5, 4};

    assert(fsort(arr, 5, sizeof(short), 4, shortcmp) == 0);
    assert(arr[0] == 1);
    assert(arr[1] == 2);
    assert(arr[2] == 3);
    assert(arr[3] == 4);
    assert(arr[4] == 5);
    //printf("testShortArray works\n");
}

void testIntArray() {
    int arr[] = {3, 1, 2, 5, 4};

    assert(fsort(arr, 5, sizeof(int), 4, intcmp) == 0);
    assert(arr[0] == 1);
    assert(arr[1] == 2);
    assert(arr[2] == 3);
    assert(arr[3] == 4);
    assert(arr[4] == 5);
    //printf("testIntArray works\n");
}

int str_cmp(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void testStringArray() {
    const char *arr[] = {"banana", "apple", "cherry"};

    assert(fsort(arr, 3, sizeof(char *), 4, str_cmp) == 0);
    assert(strcmp(arr[0], "apple") == 0);
    assert(strcmp(arr[1], "banana") == 0);
    assert(strcmp(arr[2], "cherry") == 0);
    //printf("testStringArray works\n");
}

int doublecmp(const void *a, const void *b) {
    if (*(double *)a < *(double *)b) return -1;
    if (*(double *)a > *(double *)b) return 1;
    return 0;
}

void testDoubleArray() {
    double arr[] = {3.1, 1.4, 2.2, 5.9, 4.6};

    assert(fsort(arr, 5, sizeof(double), 4, doublecmp) == 0);
    assert(arr[0] == 1.4);
    assert(arr[1] == 2.2);
    assert(arr[2] == 3.1);
    assert(arr[3] == 4.6);
    assert(arr[4] == 5.9);
    //printf("testDoubleArray works\n");
}

typedef struct {
    int id;
    char name[20];
} Person;

int personcmp(const void *a, const void *b) {
    return ((Person *)a)->id - ((Person *)b)->id;
}

void testStructArray() {
    Person arr[] = {{3, "Charlie"}, {1, "Alice"}, {2, "Bob"}};

    assert(fsort(arr, 3, sizeof(Person), 4, personcmp) == 0);
    assert(arr[0].id == 1 && strcmp(arr[0].name, "Alice") == 0);
    assert(arr[1].id == 2 && strcmp(arr[1].name, "Bob") == 0);
    assert(arr[2].id == 3 && strcmp(arr[2].name, "Charlie") == 0);
    //printf("testStructArray works\n");
}




int main(void) {
    test01();
    //printf("test 1 works\n");
    test02();
    //printf("test 2 works\n");
    test03();
    //printf("test 3 works\n");
    test04();
    //printf("test 4 works\n");
    test05();
    //printf("test 5 works\n");

    testEmptyArray();
    testSingleElementArray();
    testSortedArray();
    testIdenticalElementsArray();
    testShortArray();
    testIntArray();
    testStringArray();
    testDoubleArray();
    testStructArray();

    return 0;
}
