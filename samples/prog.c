#include <stdio.h>

int main(void) {
    int n;
    int ans = 0;

    scanf("%d", &n);
    for (int i = 0; i < n; i++)  {
        int tmp;
        scanf("%d", &tmp);
        ans += tmp;
    }

    printf("%d\n", ans);
    return 0;
}
