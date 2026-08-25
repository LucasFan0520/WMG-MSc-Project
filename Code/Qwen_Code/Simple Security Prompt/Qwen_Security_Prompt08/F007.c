// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct A {
    int time;
    char *label;
    char *note;
    struct A *next;
} A;

A *head = NULL;

int main() {
    char cmd[20];
    char label[100];
    char note[1000];
    int time;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%d %s", &time, label);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            A *n = malloc(sizeof(A));
            n->time = time;
            n->label = strdup(label);
            n->note = strdup(note);
            n->next = head;
            head = n;
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%d %s", &time, label);
            A *p = head;
            A *prev = NULL;
            while (p) {
                if (p->time == time && !strcmp(p->label, label)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    A *tmp = p;
                    p = p->next;
                    free(tmp->label);
                    free(tmp->note);
                    free(tmp);
                } else {
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", label);
            A *best = NULL;
            for (A *p = head; p; p = p->next) {
                if (!strcmp(p->label, label) && (!best || p->time < best->time)) {
                    best = p;
                }
            }
            if (best) {
                printf("%d %s %s\n", best->time, best->label, best->note);
            }
        } else if (!strcmp(cmd, "SORT")) {
            A *arr[1000];
            int n = 0;
            for (A *p = head; p; p = p->next) {
                arr[n++] = p;
            }
            for (int i = 0; i < n - 1; i++) {
                for (int j = 0; j < n - i - 1; j++) {
                    if (arr[j]->time > arr[j+1]->time || (arr[j]->time == arr[j+1]->time && strcmp(arr[j]->label, arr[j+1]->label) > 0)) {
                        A *tmp = arr[j];
                        arr[j] = arr[j+1];
                        arr[j+1] = tmp;
                    }
                }
            }
            for (int i = 0; i < n; i++) {
                printf("%d %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
            }
        } else if (!strcmp(cmd, "CLEAR")) {
            A *p = head;
            while (p) {
                A *tmp = p;
                p = p->next;
                free(tmp->label);
                free(tmp->note);
                free(tmp);
            }
            head = NULL;
        }
    }
    return 0;
}