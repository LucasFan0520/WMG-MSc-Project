// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Tab {
    char* name;
    int rows;
    int cols;
    char*** cells;
    struct Tab* next;
} Tab;

Tab* head = NULL;

Tab* find_tab(const char* name) {
    for (Tab* t = head; t; t = t->next)
        if (strcmp(t->name, name) == 0) return t;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "TABLE") == 0) {
            char* name = get_word(&p);
            char* r_str = get_word(&p);
            char* c_str = get_word(&p);
            if (name && r_str && c_str) {
                int r = atoi(r_str);
                int c = atoi(c_str);
                if (r > 0 && c > 0 && !find_tab(name)) {
                    Tab* n = malloc(sizeof(Tab));
                    n->name = strdup(name);
                    n->rows = r;
                    n->cols = c;
                    n->cells = malloc(r * sizeof(char**));
                    for (int i = 0; i < r; i++) {
                        n->cells[i] = malloc(c * sizeof(char*));
                        for (int j = 0; j < c; j++) {
                            n->cells[i][j] = NULL;
                        }
                    }
                    n->next = head;
                    head = n;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char* name = get_word(&p);
            char* r_str = get_word(&p);
            char* c_str = get_word(&p);
            Tab* t = find_tab(name);
            if (t && r_str && c_str) {
                int r = atoi(r_str);
                int c = atoi(c_str);
                if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                    if (t->cells[r][c]) free(t->cells[r][c]);
                    t->cells[r][c] = strdup(p);
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char* name = get_word(&p);
            char* r_str = get_word(&p);
            char* c_str = get_word(&p);
            Tab* t = find_tab(name);
            if (t && r_str && c_str) {
                int r = atoi(r_str);
                int c = atoi(c_str);
                if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                    if (t->cells[r][c]) printf("%s\n", t->cells[r][c]);
                    else printf("EMPTY\n");
                } else {
                    printf("EMPTY\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* name = get_word(&p);
            Tab* prev = NULL;
            for (Tab* t = head; t; prev = t, t = t->next) {
                if (strcmp(t->name, name) == 0) {
                    if (prev) prev->next = t->next;
                    else head = t->next;
                    free(t->name);
                    for (int i = 0; i < t->rows; i++) {
                        for (int j = 0; j < t->cols; j++) {
                            if (t->cells[i][j]) free(t->cells[i][j]);
                        }
                        free(t->cells[i]);
                    }
                    free(t->cells);
                    free(t);
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (Tab* t = head; t; t = t->next) {
                printf("%s\n", t->name);
            }
        }
    }
    return 0;
}