// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

char *read_word(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    int c;
    while ((c = fgetc(f)) != EOF && (c == ' ' || c == '\n' || c == '\r' || c == '\t'));
    if (c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len++] = c;
    while ((c = fgetc(f)) != EOF && c != ' ' && c != '\n' && c != '\r' && c != '\t') {
        if (len + 1 >= size) {
            size *= 2;
            buf = realloc(buf, size);
        }
        buf[len++] = c;
    }
    buf[len] = '\0';
    return buf;
}

int is_valid_num(const char *s) {
    if (!s || *s == '\0') return 0;
    char *endptr;
    strtod(s, &endptr);
    return *endptr == '\0';
}

typedef struct {
    char *account;
    double amount;
    char *memo;
} Tx;

int main(int argc, char **argv) {
    Tx *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *acc = read_word(f);
                if (!acc) break;
                char *amt_str = read_word(f);
                char *mem = read_word(f);
                if (amt_str && mem && is_valid_num(amt_str)) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(Tx));
                    }
                    arr[count].account = strdup(acc);
                    arr[count].amount = strtod(amt_str, NULL);
                    arr[count].memo = strdup(mem);
                    count++;
                }
                free(acc);
                free(amt_str);
                free(mem);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *acc = line + 4;
            char *s1 = strchr(acc, ' ');
            if (s1) {
                *s1 = '\0';
                char *amt_str = s1 + 1;
                char *s2 = strchr(amt_str, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *mem = s2 + 1;
                    if (is_valid_num(amt_str)) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            arr = realloc(arr, cap * sizeof(Tx));
                        }
                        arr[count].account = strdup(acc);
                        arr[count].amount = strtod(amt_str, NULL);
                        arr[count].memo = strdup(mem);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *acc = line + 8;
            char *s1 = strchr(acc, ' ');
            if (s1) {
                *s1 = '\0';
                char *amt_str = s1 + 1;
                if (is_valid_num(amt_str)) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(Tx));
                    }
                    arr[count].account = strdup(acc);
                    arr[count].amount = -strtod(amt_str, NULL);
                    arr[count].memo = strdup("REVERSAL");
                    count++;
                }
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            double bal = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].account, acc) == 0) {
                    bal += arr[i].amount;
                }
            }
            printf("%g\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %g %s\n", arr[i].account, arr[i].amount, arr[i].memo);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].account);
        free(arr[i].memo);
    }
    free(arr);
    return 0;
}
