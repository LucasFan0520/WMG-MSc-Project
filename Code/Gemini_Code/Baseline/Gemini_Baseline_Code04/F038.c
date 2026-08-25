// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    RLEString *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *p = line + 6;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *enc = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(RLEString));
                arr[count].name = strdup(name);
                arr[count].encoded = strdup(enc);
                count++;
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    char *p = arr[i].encoded;
                    int valid = 1;
                    while (*p) {
                        if (isdigit((unsigned char)*p)) {
                            long run_len = 0;
                            while (*p && isdigit((unsigned char)*p)) {
                                run_len = run_len * 10 + (*p - '0');
                                if (run_len > 100000) {
                                    valid = 0;
                                    break;
                                }
                                p++;
                            }
                            if (!valid || !*p) {
                                valid = 0;
                                break;
                            }
                            char ch = *p;
                            p++;
                            for (int j = 0; j < run_len; j++) {
                                if (ch == ' ') {
                                    putchar('_');
                                } else {
                                    putchar(ch);
                                }
                            }
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    if (!valid) {
                        printf(" INVALID");
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].encoded);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].encoded);
    }
    free(arr);
    return 0;
}
