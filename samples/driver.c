#include <stdio.h>

int main(void) {
    printf("%d\n", 10);
    for (int i = 0; i < 10; i++) {
        printf("%d\n", i + 1);
    }
    int n;
    scanf("%d", &n);
    if (n == 55)
        return 0;
    return 1;
}
