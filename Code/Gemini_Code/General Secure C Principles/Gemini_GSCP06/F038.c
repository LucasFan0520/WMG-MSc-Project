// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main(void) {
    RLEString *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *encoded = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RLEString *next = realloc(list, capacity * sizeof(RLEString));
                    if (!next) break;
                    list = next;
                }
                list[count].name = strdup(name);
                list[count].encoded = strdup(encoded);
                count++;
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    char *enc = list[i].encoded;
                    size_t k = 0;
                    int valid = 1;
                    while (enc[k] != '\0') {
                        if (enc[k] >= '0' && enc[k] <= '9') {
                            long long num = 0;
                            while (enc[k] >= '0' && enc[k] <= '9') {
                                num = num * 10 + (enc[k] - '0');
                                if (num > 100000) {
                                    valid = 0;
                                    break;
                                }
                                k++;
                            }
                            if (!valid || enc[k] == '\0') {
                                valid = 0;
                                break;
                            }
                            char ch = enc[k];
                            for (long long r = 0; r < num; r++) {
                                if (ch == ' ') putchar('_');
                                else putchar(ch);
                            }
                            k++;
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].encoded);
    }
    free(list);
    free(line);
    return 0;
}
