#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *password = "abcdefg";

int main() {
    char *pwd = (char *)malloc(100);
    printf("enter password: ");
    scanf("%s", pwd);
    if (strcmp(pwd, password) == 0) {
        printf("success!\n");
    } else {
        printf("incorrect password!\n");
    }
    free(pwd);
    return 0;
}