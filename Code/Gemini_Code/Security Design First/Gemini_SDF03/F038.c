// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main(void) {
    RLEString *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *encoded = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                encoded = p;
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].encoded);
                    arr[i].encoded = strdup(encoded);
                    if (!arr[i].encoded) exit(1);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RLEString *tmp = realloc(arr, capacity * sizeof(RLEString));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                arr[count].encoded = strdup(encoded);
                if (!arr[count].name || !arr[count].encoded) exit(1);
                count++;
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    char *t = arr[i].encoded;
                    int valid = 1;
                    while (*t) {
                        if (isdigit((unsigned char)*t)) {
                            long long c_val = 0;
                            while (*t && isdigit((unsigned char)*t)) {
                                c_val = c_val * 10 + (*t - '0');
                                if (c_val > 1000000) {
                                    valid = 0;
                                    break;
                                }
                                t++;
                            }
                            if (!valid || *t == '\0') {
                                valid = 0;
                                break;
                            }
                            char ch = *t;
                            for (long long k = 0; k < c_val; k++) {
                                if (ch == ' ') putchar('_');
                                else putchar(ch);
                            }
                            t++;
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    if (!valid) {
                        printf("INVALID");
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].encoded);
    }
    free(arr);
    free(line);
    return 0;
}
