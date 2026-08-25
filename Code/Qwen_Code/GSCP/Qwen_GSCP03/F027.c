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

void free_list() {
    while (head) {
        Rem *r = head; head = head->next;
        free(r->title); free(r->note); free(r);
    }
}

Rem *find_rem(const char *title) {
    for (Rem *r = head; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], title[256], note[4096] = "";
        int prio, delta, n;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s %4095[^\n]", &prio, title, note) < 2) continue;
            Rem *r = malloc(sizeof(Rem));
            r->priority = prio; r->title = strdup(title); r->note = strdup(note);
            r->next = head; head = r;
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &delta) != 2) continue;
            Rem *r = find_rem(title);
            if (r) r->priority += delta;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Rem *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    Rem *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->title); free(tmp->note); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %d", &n) != 1) continue;
            int count = 0;
            for (Rem *r = head; r; r = r->next) count++;
            Rem **arr = malloc(count * sizeof(Rem*));
            int i = 0;
            for (Rem *r = head; r; r = r->next) arr[i++] = r;
            for (int a = 0; a < count - 1; a++) {
                for (int b = a + 1; b < count; b++) {
                    if (arr[a]->priority < arr[b]->priority) {
                        Rem *tmp = arr[a]; arr[a] = arr[b]; arr[b] = tmp;
                    }
                }
            }
            int limit = n < count ? n : count;
            for (int a = 0; a < limit; a++) {
                printf("%d %s %s\n", arr[a]->priority, arr[a]->title, arr[a]->note);
            }
            free(arr);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Rem *r = head; r; r = r->next) {
                printf("%d %s %s\n", r->priority, r->title, r->note);
            }
        }
    }
    free_list();
    return 0;
}