// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

typedef struct { char *title; char *note; int status; } Task; // 0:todo, 1:doing, 2:done
Task *tasks = NULL;
size_t tc = 0, tcap = 0;

void add_task(const char *title, const char *note, int status) {
    for (size_t i = 0; i < tc; i++) {
        if (strcmp(tasks[i].title, title) == 0) {
            tasks[i].status = status;
            char *tmp = strdup(note);
            if (!tmp) { perror("strdup"); exit(1); }
            free(tasks[i].note);
            tasks[i].note = tmp;
            return;
        }
    }
    if (tc == tcap) {
        size_t nc = tcap ? tcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Task)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Task *tmp = realloc(tasks, nc * sizeof(Task));
        if (!tmp) { perror("realloc"); exit(1); }
        tasks = tmp;
        tcap = nc;
    }
    tasks[tc].title = strdup(title);
    tasks[tc].note = strdup(note);
    tasks[tc].status = status;
    if (!tasks[tc].title || !tasks[tc].note) { perror("strdup"); exit(1); }
    tc++;
}

void rem_task(const char *title) {
    for (size_t i = 0; i < tc; i++) {
        if (strcmp(tasks[i].title, title) == 0) {
            free(tasks[i].title); free(tasks[i].note);
            tasks[i] = tasks[--tc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "TODO") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) != 1) { free(line); continue; }
            char *note = strchr(line, ' ');
            if (note) { note = strchr(note + 1, ' '); if (note) note++; }
            if (!note) note = "";
            add_task(title, note, 0);
        } else if (strcmp(cmd, "START") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                for (size_t i = 0; i < tc; i++) if (strcmp(tasks[i].title, title) == 0) tasks[i].status = 1;
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                for (size_t i = 0; i < tc; i++) if (strcmp(tasks[i].title, title) == 0) tasks[i].status = 2;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) rem_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                for (size_t i = 0; i < tc; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        const char *s = tasks[i].status == 0 ? "TODO" : (tasks[i].status == 1 ? "DOING" : "DONE");
                        printf("%s %s\n", s, tasks[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("TODO:\n");
            for (size_t i = 0; i < tc; i++) if (tasks[i].status == 0) printf("%s %s\n", tasks[i].title, tasks[i].note);
            printf("DOING:\n");
            for (size_t i = 0; i < tc; i++) if (tasks[i].status == 1) printf("%s %s\n", tasks[i].title, tasks[i].note);
            printf("DONE:\n");
            for (size_t i = 0; i < tc; i++) if (tasks[i].status == 2) printf("%s %s\n", tasks[i].title, tasks[i].note);
        }
        free(line);
    }
    for (size_t i = 0; i < tc; i++) { free(tasks[i].title); free(tasks[i].note); }
    free(tasks);
    return 0;
}