// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Act {
    int time;
    char *label, *note;
    struct Act *next;
} Act;

Act *head = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main() {
    char cmd[20], label[256], note[1024];
    int time;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d %s", &time, label); read_rest(note);
            Act *a = malloc(sizeof(Act));
            a->time = time; a->label = strdup(label); a->note = strdup(note); a->next = NULL;
            if (!head) head = a;
            else { Act *c = head; while (c->next) c = c->next; c->next = a; }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d %s", &time, label);
            Act *prev = NULL, *curr = head;
            while (curr) {
                if (curr->time == time && strcmp(curr->label, label) == 0) {
                    Act *next = curr->next;
                    if (prev) prev->next = next; else head = next;
                    free(curr->label); free(curr->note); free(curr);
                    curr = next;
                } else { prev = curr; curr = curr->next; }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", label);
            for (Act *a = head; a; a = a->next)
                if (strcmp(a->label, label) == 0) { printf("%d %s %s\n", a->time, a->label, a->note); break; }
        } else if (strcmp(cmd, "SORT") == 0) {
            int n = 0;
            for (Act *a = head; a; a = a->next) n++;
            Act **arr = malloc(n * sizeof(Act*));
            Act *a = head;
            for (int i = 0; i < n; i++) { arr[i] = a; a = a->next; }
            for (int i = 0; i < n - 1; i++)
                for (int j = 0; j < n - i - 1; j++) {
                    int swap = 0;
                    if (arr[j]->time > arr[j+1]->time) swap = 1;
                    else if (arr[j]->time == arr[j+1]->time && strcmp(arr[j]->label, arr[j+1]->label) > 0) swap = 1;
                    if (swap) { Act *t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t; }
                }
            for (int i = 0; i < n; i++) printf("%d %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
            free(arr);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            while (head) { Act *next = head->next; free(head->label); free(head->note); free(head); head = next; }
        }
    }
    return 0;
}