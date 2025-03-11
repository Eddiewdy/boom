#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Allocating memory...\n");
    
    int *arr = (int *)malloc(10 * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        arr[i] = i;
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    printf("Memory freed.\n");

    return 0;
}
