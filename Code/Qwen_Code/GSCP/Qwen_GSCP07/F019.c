// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Trans { char *acc, *memo; int amount; struct Trans *next; } Trans;
Trans *head = NULL;

void add_trans(const char *acc, int amount, const char *memo) {
    Trans *t = malloc(sizeof(Trans));
    t->acc = strdup(acc);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = head;
    head = t;
}

void reverse_trans(const char *acc, int amount) {
    Trans **p = &head;
    while (*p) {
        if (strcmp((*p)->acc, acc) == 0 && (*p)->amount == amount) {
            Trans *tmp = *p;
            *p = tmp->next;
            free(tmp->acc);
            free(tmp->memo);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void balance_acc(const char *acc) {
    int bal = 0;
    for (Trans *t = head; t; t = t->next) {
        if (strcmp(t->acc, acc) == 0) bal += t->amount;
    }
    printf("%d\n", bal);
}

void report() {
    for (Trans *t = head; t; t = t->next) printf("%s %d %s\n", t->acc, t->amount, t->memo);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char acc[256], memo[1024] = "";
        int amount;
        if (sscanf(line, "%255s %d", acc, &amount) == 2) {
            char *t = strchr(line, ' ');
            if (t) { t = strchr(t + 1, ' '); if (t) strcpy(memo, t + 1); }
            add_trans(acc, amount, memo);
        }
    }
    fclose(f);
}

void cleanup() {
    while (head) {
        Trans *tmp = head->next;
        free(head->acc);
        free(head->memo);
        free(head);
        head = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], acc[256], memo[1024] = "";
        int amount;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d", acc, &amount) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) strcpy(memo, t + 1); } }
                add_trans(acc, amount, memo);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %255s %d", acc, &amount) == 2) reverse_trans(acc, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (sscanf(line, "%*s %255s", acc) == 1) balance_acc(acc);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    cleanup();
    return 0;
}