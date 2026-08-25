// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct T {
    char *title;
    char *note;
    struct T *next;
} T;

T *todo = NULL;
T *doing = NULL;
T *done = NULL;

void add_to(T **list, char *title, char *note) {
    T *nt = malloc(sizeof(T));
    nt->title = strdup(title);
    nt->note = strdup(note);
    nt->next = *list;
    *list = nt;
}

T* remove_from(T **list, char *title) {
    T *p = *list;
    T *prev = NULL;
    while (p) {
        if (!strcmp(p->title, title)) {
            if (prev) {
                prev->next = p->next;
            } else {
                *list = p->next;
            }
            p->next = NULL;
            return p;
        }
        prev = p;
        p = p->next;
    }
    return NULL;
}

void print_list(T *list) {
    for (T *p = list; p; p = p->next) {
        printf("%s %s\n", p->title, p->note);
    }
}

int main() {
    char cmd[20];
    char title[100];
    char note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "TODO")) {
            scanf("%s", title);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            add_to(&todo, title, note);
        } else if (!strcmp(cmd, "START")) {
            scanf("%s", title);
            T *t = remove_from(&todo, title);
            if (t) {
                t->next = doing;
                doing = t;
            }
        } else if (!strcmp(cmd, "DONE")) {
            scanf("%s", title);
            T *t = remove_from(&doing, title);
            if (t) {
                t->next = done;
                done = t;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s", title);
            T *t = remove_from(&todo, title);
            if (!t) {
                t = remove_from(&doing, title);
            }
            if (!t) {
                t = remove_from(&done, title);
            }
            if (t) {
                free(t->title);
                free(t->note);
                free(t);
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", title);
            T *p = todo;
            while (p && strcmp(p->title, title)) {
                p = p->next;
            }
            if (p) {
                printf("todo %s\n", p->note);
                continue;
            }
            p = doing;
            while (p && strcmp(p->title, title)) {
                p = p->next;
            }
            if (p) {
                printf("doing %s\n", p->note);
                continue;
            }
            p = done;
            while (p && strcmp(p->title, title)) {
                p = p->next;
            }
            if (p) {
                printf("done %s\n", p->note);
            }
        } else if (!strcmp(cmd, "REPORT")) {
            printf("todo:\n");
            print_list(todo);
            printf("doing:\n");
            print_list(doing);
            printf("done:\n");
            print_list(done);
        }
    }
    return 0;
}