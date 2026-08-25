// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char *argv[]) {
    Transaction *journal = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char acc[128], mem[128];
            double amt;
            while (fscanf(fp, "%127s %lf %127s", acc, &amt, mem) == 3) {
                journal = realloc(journal, (count + 1) * sizeof(Transaction));
                journal[count].account = strdup(acc);
                journal[count].amount = amt;
                journal[count].memo = strdup(mem);
                count++;
            }
            fclose(fp);
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
            char acc[128], mem[128];
            double amt;
            if (sscanf(line + 4, "%127s %lf %127s", acc, &amt, mem) == 3) {
                journal = realloc(journal, (count + 1) * sizeof(Transaction));
                journal[count].account = strdup(acc);
                journal[count].amount = amt;
                journal[count].memo = strdup(mem);
                count++;
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char acc[128];
            double amt;
            if (sscanf(line + 8, "%127s %lf", acc, &amt) == 2) {
                journal = realloc(journal, (count + 1) * sizeof(Transaction));
                journal[count].account = strdup(acc);
                journal[count].amount = -amt;
                journal[count].memo = strdup("REVERSAL");
                count++;
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            double bal = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(journal[i].account, acc) == 0) {
                    bal += journal[i].amount;
                }
            }
            printf("%.2lf\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2lf %s\n", journal[i].account, journal[i].amount, journal[i].memo);
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
