// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rem {
    int priority;
    char *title;
    char *note;
    struct Rem *next;
} Rem;

Rem *head = NULL;

int main() {
    char cmd[20];
    char title[100];
    char note[1000];
    int priority, delta, n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%d %s", &priority, title);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            Rem *r = malloc(sizeof(Rem));
            r->priority = priority;
            r->title = strdup(title);
            r->note = strdup(note);
            r->next = head;
            head = r;
        } else if (!strcmp(cmd, "BUMP")) {
            scanf("%s %d", title, &delta);
            for (Rem *r = head; r; r = r->next) {
                if (!strcmp(r->title, title)) {
                    r->priority += delta;
                    break;
                }
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s", title);
            Rem *p = head;
            Rem *prev = NULL;
            while (p) {
                if (!strcmp(p->title, title)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    free(p->title);
                    free(p->note);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "TOP")) {
            scanf("%d", &n);
            Rem *arr[1000];
            int count = 0;
            for (Rem *r = head; r; r = r->next) {
                arr[count++] = r;
            }
            for (int i = 0; i < count - 1; i++) {
                for (int j = 0; j < count - i - 1; j++) {
                    if (arr[j]->priority < arr[j+1]->priority) {
                        Rem *tmp = arr[j];
                        arr[j] = arr[j+1];
                        arr[j+1] = tmp;
                    }
                }
            }
            for (int i = 0; i < n && i < count; i++) {
                printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (Rem *r = head; r; r = r->next) {
                printf("%d %s %s\n", r->priority, r->title, r->note);
            }
        }
    }
    return 0;
}