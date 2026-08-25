// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line_file(FILE *f) {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *account; int amount; char *memo; } Trans;
Trans *trans = NULL;
size_t tc = 0, tcap = 0;

void add_trans(const char *account, int amount, const char *memo) {
    if (tc == tcap) {
        size_t nc = tcap ? tcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Trans)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Trans *tmp = realloc(trans, nc * sizeof(Trans));
        if (!tmp) { perror("realloc"); exit(1); }
        trans = tmp;
        tcap = nc;
    }
    trans[tc].account = strdup(account);
    trans[tc].amount = amount;
    trans[tc].memo = strdup(memo);
    if (!trans[tc].account || !trans[tc].memo) { perror("strdup"); exit(1); }
    tc++;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line_file(f)) != NULL) {
        char account[256], memo[256];
        int amount;
        if (sscanf(line, "%255s %d %255s", account, &amount, memo) == 3) {
            add_trans(account, amount, memo);
        }
        free(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char account[256], memo[256];
            int amount;
            if (sscanf(line, "%*s %255s %d %255s", account, &amount, memo) == 3) add_trans(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char account[256]; int amount;
            if (sscanf(line, "%*s %255s %d", account, &amount) == 2) {
                for (size_t i = 0; i < tc; i++) {
                    if (strcmp(trans[i].account, account) == 0 && trans[i].amount == amount) {
                        trans[i].amount = -trans[i].amount;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char account[256];
            if (sscanf(line, "%*s %255s", account) == 1) {
                long sum = 0;
                for (size_t i = 0; i < tc; i++) {
                    if (strcmp(trans[i].account, account) == 0) sum += trans[i].amount;
                }
                printf("%ld\n", sum);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < tc; i++) printf("%s %d %s\n", trans[i].account, trans[i].amount, trans[i].memo);
        }
        free(line);
    }
    for (size_t i = 0; i < tc; i++) { free(trans[i].account); free(trans[i].memo); }
    free(trans);
    return 0;
}