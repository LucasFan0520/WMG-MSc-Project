// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *read_token(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && (c == ' ' || c == '\n' || c == '\r' || c == '\t'));
    if (c == EOF) { free(buf); return NULL; }
    buf[len++] = c;
    while ((c = fgetc(f)) != EOF && c != ' ' && c != '\n' && c != '\r' && c != '\t') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *account;
    double amount;
    char *memo;
} Trans;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f) return 1;
    Trans *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *acc = read_token(f);
        if (!acc) break;
        char *amt_str = read_token(f);
        char *memo = read_token(f);
        if (!amt_str || !memo) {
            free(acc);
            free(amt_str);
            free(memo);
            break;
        }
        char *endptr;
        double amt = strtod(amt_str, &endptr);
        if (*endptr == '\0' && amt_str != endptr) {
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Trans *nb = realloc(list, cap * sizeof(Trans));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].account = strdup(acc);
                list[count].amount = amt;
                list[count].memo = strdup(memo);
                count++;
            }
        }
        free(acc);
        free(amt_str);
        free(memo);
    }
    fclose(f);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *acc = arg1;
            p = arg1;
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
            if (*endptr == '\0' && amt_str != endptr) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Trans *nb = realloc(list, cap * sizeof(Trans));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].account = strdup(acc);
                    list[count].amount = amt;
                    list[count].memo = strdup(memo);
                    count++;
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *amt_str = p;
            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (*endptr == '\0' && amt_str != endptr) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Trans *nb = realloc(list, cap * sizeof(Trans));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].account = strdup(acc);
                    list[count].amount = -amt;
                    list[count].memo = strdup("REVERSAL");
                    count++;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            double bal = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].account, arg1) == 0) {
                    bal += list[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f %s\n", list[i].account, list[i].amount, list[i].memo);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].account);
        free(list[i].memo);
    }
    free(list);
    return 0;
}
