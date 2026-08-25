// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title, *note;
    int status;
    struct Task *next;
} Task;

Task *head = NULL;

void free_list() {
    while (head) {
        Task *t = head; head = head->next;
        free(t->title); free(t->note); free(t);
    }
}

Task *find_task(const char *title) {
    for (Task *t = head; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

const char *status_str(int s) {
    return s == 0 ? "todo" : (s == 1 ? "doing" : "done");
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], title[256], note[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "TODO") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, note) < 1) continue;
            if (find_task(title)) continue;
            Task *t = malloc(sizeof(Task));
            t->title = strdup(title); t->note = strdup(note); t->status = 0;
            t->next = head; head = t;
        } else if (strcmp(cmd, "START") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Task *t = find_task(title);
            if (t && t->status == 0) t->status = 1;
        } else if (strcmp(cmd, "DONE") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Task *t = find_task(title);
            if (t && t->status == 1) t->status = 2;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Task *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    Task *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->title); free(tmp->note); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Task *t = find_task(title);
            if (t) printf("%s %s\n", status_str(t->status), t->note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int s = 0; s < 3; s++) {
                printf("%s:\n", status_str(s));
                for (Task *t = head; t; t = t->next)
                    if (t->status == s) printf("  %s %s\n", t->title, t->note);
            }
        }
    }
    free_list();
    return 0;
}