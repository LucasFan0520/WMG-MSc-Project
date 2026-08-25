/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

char *read_word(FILE *fp) {
    size_t len = 0;
    size_t cap = 0;
    char *buf = NULL;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (len > 0) break;
            continue;
        }
        if (len + 1 >= cap) {
            size_t new_cap = cap == 0 ? 8 : cap * 2;
            char *temp = realloc(buf, new_cap);
            if (!temp) {
                free(buf);
                return NULL;
            }
            buf = temp;
            cap = new_cap;
        }
        buf[len++] = c;
    }
    if (len > 0) {
        buf[len] = '\0';
        return buf;
    }
    free(buf);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    Transaction *journal = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *acc = read_word(fp);
            if (!acc) break;
            char *amt_str = read_word(fp);
            if (!amt_str) {
                free(acc);
                break;
            }
            char *memo = read_word(fp);
            if (!memo) {
                free(acc);
                free(amt_str);
                break;
            }

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str && *endptr == '\0') {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Transaction *temp = realloc(journal, new_cap * sizeof(Transaction));
                    if (!temp) {
                        free(acc); free(amt_str); free(memo);
                        break;
                    }
                    journal = temp;
                    capacity = new_cap;
                }
                journal[count].account = strdup(acc);
                journal[count].amount = amt;
                journal[count].memo = strdup(memo);
                if (journal[count].account && journal[count].memo) {
                    count++;
                }
            }
            free(acc);
            free(amt_str);
            free(memo);
        }
        fclose(fp);
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *amt_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *memo = p;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str && *endptr == '\0') {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Transaction *temp = realloc(journal, new_cap * sizeof(Transaction));
                    if (!temp) break;
                    journal = temp;
                    capacity = new_cap;
                }
                journal[count].account = strdup(acc);
                journal[count].amount = amt;
                journal[count].memo = strdup(memo);
                if (journal[count].account && journal[count].memo) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *amt_str = p;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str && *endptr == '\0') {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Transaction *temp = realloc(journal, new_cap * sizeof(Transaction));
                    if (!temp) break;
                    journal = temp;
                    capacity = new_cap;
                }
                journal[count].account = strdup(acc);
                journal[count].amount = -amt;
                journal[count].memo = strdup("REVERSAL");
                if (journal[count].account && journal[count].memo) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = p;
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(journal[i].account, acc) == 0) {
                    bal += journal[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f %s\n", journal[i].account, journal[i].amount, journal[i].memo);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(journal[i].account);
        free(journal[i].memo);
    }
    free(journal);
    return 0;
}
