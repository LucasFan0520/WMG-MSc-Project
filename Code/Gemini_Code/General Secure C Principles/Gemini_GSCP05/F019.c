// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

char *read_token(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && (c == ' ' || c == '\n' || c == '\r' || c == '\t'));
    if (c == EOF) {
        free(str);
        return NULL;
    }
    str[len++] = (char)c;
    while ((c = fgetc(stream)) != EOF && c != ' ' && c != '\n' && c != '\r' && c != '\t') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;
    Transaction *journal = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *acc = read_token(fp);
        if (!acc) break;
        char *amt_str = read_token(fp);
        char *mem = read_token(fp);
        if (amt_str && mem) {
            char *endptr;
            double val = strtod(amt_str, &endptr);
            if (endptr != amt_str && *endptr == '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Transaction *new_j = realloc(journal, capacity * sizeof(Transaction));
                    if (new_j) journal = new_j;
                }
                if (count < capacity) {
                    journal[count].account = acc;
                    journal[count].amount = val;
                    journal[count].memo = mem;
                    count++;
                    acc = NULL;
                    mem = NULL;
                }
            }
        }
        free(acc);
        free(amt_str);
        free(mem);
    }
    fclose(fp);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *acc_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *amt_start = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *mem_start = p + 1;
                    while (*mem_start == ' ') mem_start++;
                    char *endptr;
                    double val = strtod(amt_start, &endptr);
                    if (endptr != amt_start) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Transaction *new_j = realloc(journal, capacity * sizeof(Transaction));
                            if (new_j) journal = new_j;
                        }
                        if (count < capacity) {
                            journal[count].account = strdup(acc_start);
                            journal[count].amount = val;
                            journal[count].memo = strdup(mem_start);
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *acc_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *amt_start = p + 1;
                while (*amt_start == ' ') amt_start++;
                char *endptr;
                double val = strtod(amt_start, &endptr);
                if (endptr != amt_start) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Transaction *new_j = realloc(journal, capacity * sizeof(Transaction));
                        if (new_j) journal = new_j;
                    }
                    if (count < capacity) {
                        journal[count].account = strdup(acc_start);
                        journal[count].amount = -val;
                        journal[count].memo = strdup("REVERSAL");
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc_start = line + 8;
            while (*acc_start == ' ') acc_start++;
            double bal = 0.0;
            for (int i = 0; i < count; i++) {
                if (strcmp(journal[i].account, acc_start) == 0) {
                    bal += journal[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f %s\n", journal[i].account, journal[i].amount, journal[i].memo);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(journal[i].account);
        free(journal[i].memo);
    }
    free(journal);
    return 0;
}
