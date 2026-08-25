// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    int list; // 0=todo, 1=doing, 2=done
    struct Task *next;
} Task;

Task *head = NULL;

Task *find_task(const char *title) {
    Task *t = head;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_task(const char *title, const char *note, int list) {
    if (find_task(title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->list = list;
    t->next = head;
    head = t;
}

void move_task(const char *title, int new_list) {
    Task *t = find_task(title);
    if (!t) return;
    t->list = new_list;
}

void remove_task(const char *title) {
    Task *prev = NULL;
    Task *t = head;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else head = t->next;
            free(t->title);
            free(t->note);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void find_task_print(const char *title) {
    Task *t = find_task(title);
    if (!t) return;
    const char *listname[] = {"todo", "doing", "done"};
    printf("%s ", listname[t->list]);
    for (char *p = t->note; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void report_all(void) {
    const char *listname[] = {"todo", "doing", "done"};
    for (int l = 0; l < 3; l++) {
        printf("%s\n", listname[l]);
        Task *t = head;
        while (t) {
            if (t->list == l) {
                printf("%s ", t->title);
                for (char *p = t->note; *p; p++) {
                    putchar(*p == ' ' ? '_' : *p);
                }
                putchar('\n');
            }
            t = t->next;
        }
    }
}

void free_all(void) {
    while (head) {
        Task *next = head->next;
        free(head->title);
        free(head->note);
        free(head);
        head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "TODO") == 0) {
            char title[256], note[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", title, note) == 2) {
                add_task(title, note, 0);
            }
        } else if (strcmp(cmd, "START") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                move_task(title, 1);
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                move_task(title, 2);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                remove_task(title);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                find_task_print(title);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    free(line);
    free_all();
    return 0;
}