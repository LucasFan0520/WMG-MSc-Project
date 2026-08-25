// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rem {
    char *title, *note;
    int priority;
    int order;
    struct Rem *next;
} Rem;

Rem *head = NULL;
int global_order = 0;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

int main() {
    char cmd[20], title[256], note[1024];
    int prio, delta, n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d", &prio); scanf("%s", title); read_rest(note, sizeof(note));
            Rem *r = malloc(sizeof(Rem));
            r->title = strdup(title); r->note = strdup(note); r->priority = prio; r->order = global_order++; r->next = head; head = r;
        } else if (strcmp(cmd, "BUMP") == 0) {
            scanf("%s %d", title, &delta);
            for (Rem *r = head; r; r = r->next) if (strcmp(r->title, title) == 0) { r->priority += delta; break; }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", title);
            Rem *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->title); free(curr->note); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            scanf("%d", &n);
            int count = 0;
            for (Rem *r = head; r; r = r->next) count++;
            Rem **arr = malloc(count * sizeof(Rem*));
            Rem *r = head;
            for (int i = 0; i < count; i++) { arr[i] = r; r = r->next; }
            for (int i = 0; i < count - 1; i++)
                for (int j = 0; j < count - i - 1; j++) {
                    int swap = 0;
                    if (arr[j]->priority < arr[j+1]->priority) swap = 1;
                    else if (arr[j]->priority == arr[j+1]->priority && arr[j]->order > arr[j+1]->order) swap = 1;
                    if (swap) { Rem *t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t; }
                }
            int lim = n < count ? n : count;
            for (int i = 0; i < lim; i++) printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
            free(arr);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Rem *r = head; r; r = r->next) printf("%d %s %s\n", r->priority, r->title, r->note);
        }
    }
    return 0;
}