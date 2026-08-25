// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Tab {
    char *name;
    int rows;
    int cols;
    char ***cells;
    struct Tab *next;
} Tab;

Tab *tabs = NULL;

Tab *find_tab(const char *name) {
    for (Tab *t = tabs; t; t = t->next) {
        if (strcmp(t->name, name) == 0) return t;
    }
    return NULL;
}

void create_tab(const char *name, int rows, int cols) {
    if (find_tab(name) || rows <= 0 || cols <= 0) return;
    Tab *t = malloc(sizeof(Tab));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = NULL;
        }
    }
    t->next = tabs;
    tabs = t;
}

void set_cell(const char *name, int r, int c, const char *val) {
    Tab *t = find_tab(name);
    if (!t || r < 0 || r >= t->rows || c < 0 || c >= t->cols) return;
    free(t->cells[r][c]);
    t->cells[r][c] = strdup(val);
}

void get_cell(const char *name, int r, int c) {
    Tab *t = find_tab(name);
    if (!t || r < 0 || r >= t->rows || c < 0 || c >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    if (t->cells[r][c]) printf("%s\n", t->cells[r][c]);
    else printf("EMPTY\n");
}

void delete_tab(const char *name) {
    Tab *prev = NULL, *curr = tabs;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else tabs = curr->next;
            for (int i = 0; i < curr->rows; i++) {
                for (int j = 0; j < curr->cols; j++) {
                    free(curr->cells[i][j]);
                }
                free(curr->cells[i]);
            }
            free(curr->cells);
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_tabs() {
    for (Tab *t = tabs; t; t = t->next) {
        printf("%s\n", t->name);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        int r = 0, c = 0;
        sscanf(line, "%15s %255s %255s %d", cmd, arg1, arg2, &r);
        
        if (strcmp(cmd, "TABLE") == 0) {
            sscanf(line, "%*s %*s %d %d", &r, &c);
            create_tab(arg1, r, c);
        }
        else if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %*s %d %d", &r, &c);
            char *val = strchr(line, ' ');
            if (val) {
                val++; while(*val==' ')val++;
                val = strchr(val, ' '); if(val){val++; while(*val==' ')val++;}
                val = strchr(val, ' '); if(val){val++; while(*val==' ')val++;}
                val = strchr(val, ' '); if(val){val++; while(*val==' ')val++;}
                if (val) set_cell(arg1, r, c, val);
            }
        }
        else if (strcmp(cmd, "GET") == 0) {
            sscanf(line, "%*s %*s %d %d", &r, &c);
            get_cell(arg1, r, c);
        }
        else if (strcmp(cmd, "DELETE") == 0) delete_tab(arg1);
        else if (strcmp(cmd, "TABLES") == 0) list_tabs();
        
        free(line);
    }
    return 0;
}