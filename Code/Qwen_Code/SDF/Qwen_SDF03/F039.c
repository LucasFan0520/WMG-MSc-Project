// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells;
    struct Table *next;
} Table;

Table *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Table *find_table(const char *name) {
    for (Table *t = head; t; t = t->next)
        if (strcmp(t->name, name) == 0) return t;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = get_token(&ptr);
            char *rstr = get_token(&ptr);
            char *cstr = get_token(&ptr);
            if (name && rstr && cstr && !find_table(name)) {
                int rows = atoi(rstr);
                int cols = atoi(cstr);
                if (rows > 0 && cols > 0) {
                    Table *t = malloc(sizeof(Table));
                    if (t) {
                        t->name = strdup(name);
                        t->rows = rows;
                        t->cols = cols;
                        t->cells = malloc(rows * sizeof(char **));
                        if (t->cells) {
                            for (int i = 0; i < rows; i++) {
                                t->cells[i] = calloc(cols, sizeof(char *));
                            }
                        }
                        t->next = head;
                        head = t;
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = get_token(&ptr);
            char *rstr = get_token(&ptr);
            char *cstr = get_token(&ptr);
            if (name && rstr && cstr) {
                Table *t = find_table(name);
                if (t) {
                    int r = atoi(rstr);
                    int c = atoi(cstr);
                    if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                        free(t->cells[r][c]);
                        t->cells[r][c] = strdup(ptr);
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = get_token(&ptr);
            char *rstr = get_token(&ptr);
            char *cstr = get_token(&ptr);
            if (name && rstr && cstr) {
                Table *t = find_table(name);
                if (t) {
                    int r = atoi(rstr);
                    int c = atoi(cstr);
                    if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                        if (t->cells[r][c]) printf("%s\n", t->cells[r][c]);
                        else printf("EMPTY\n");
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Table *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        for (int i = 0; i < curr->rows; i++) {
                            for (int j = 0; j < curr->cols; j++) {
                                free(curr->cells[i][j]);
                            }
                            free(curr->cells[i]);
                        }
                        free(curr->cells);
                        free(curr->name);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (Table *t = head; t; t = t->next) {
                printf("%s\n", t->name);
            }
        }
    }
    while (head) {
        Table *next = head->next;
        for (int i = 0; i < head->rows; i++) {
            for (int j = 0; j < head->cols; j++) {
                free(head->cells[i][j]);
            }
            free(head->cells[i]);
        }
        free(head->cells);
        free(head->name);
        free(head);
        head = next;
    }
    return 0;
}