// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *account;
    double balance;
} AccountInfo;

char *read_line_from_file(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

char *read_line() {
    return read_line_from_file(stdin);
}

int is_valid_float(const char *s) {
    if (!*s) return 0;
    if (*s == '-' || *s == '+') s++;
    int dot_count = 0;
    int digit_count = 0;
    while (*s) {
        if (isdigit((unsigned char)*s)) {
            digit_count++;
        } else if (*s == '.') {
            dot_count++;
            if (dot_count > 1) return 0;
        } else {
            return 0;
        }
        s++;
    }
    return digit_count > 0;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    AccountInfo *arr = NULL;
    int count = 0;
    if (f) {
        char acc[256], amt_s[256], memo[256];
        while (fscanf(f, "%255s %255s %255s", acc, amt_s, memo) == 3) {
            if (is_valid_float(amt_s)) {
                double val = atof(amt_s);
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].account, acc) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    arr = realloc(arr, (count + 1) * sizeof(AccountInfo));
                    arr[count].account = strdup(acc);
                    arr[count].balance = 0.0;
                    found = count;
                    count++;
                }
                arr[found].balance += val;
            }
        }
        fclose(f);
    }
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *amt_s = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    if (is_valid_float(amt_s)) {
                        double val = atof(amt_s);
                        int found = -1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(arr[i].account, acc) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (found == -1) {
                            arr = realloc(arr, (count + 1) * sizeof(AccountInfo));
                            arr[count].account = strdup(acc);
                            arr[count].balance = 0.0;
                            found = count;
                            count++;
                        }
                        arr[found].balance += val;
                    }
                }
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *p = line + 8;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *amt_s = p + 1;
                if (is_valid_float(amt_s)) {
                    double val = atof(amt_s);
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].account, acc) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found == -1) {
                        arr = realloc(arr, (count + 1) * sizeof(AccountInfo));
                        arr[count].account = strdup(acc);
                        arr[count].balance = 0.0;
                        found = count;
                        count++;
                    }
                    arr[found].balance -= val;
                }
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            double bal = 0.0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].account, acc) == 0) {
                    bal = arr[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f\n", arr[i].account, arr[i].balance);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].account);
    }
    free(arr);
    return 0;
}
