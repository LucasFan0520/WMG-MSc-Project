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

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Task *find_task(const char *title) {
    for (Task *t = head; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

const char *status_name(int s) {
    return s == 0 ? "todo" : (s == 1 ? "doing" : "done");
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = get_token(&ptr);
            if (title && !find_task(title)) {
                Task *t = malloc(sizeof(Task));
                if (t) { t->title = strdup(title); t->note = strdup(ptr); t->status = 0; t->next = head; head = t; }
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = get_token(&ptr);
            if (title) { Task *t = find_task(title); if (t) t->status = 1; }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = get_token(&ptr);
            if (title) { Task *t = find_task(title); if (t) t->status = 2; }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Task *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->title, title) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->title); free(curr->note); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Task *t = find_task(title);
                if (t) printf("%s %s\n", status_name(t->status), t->note);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int s = 0; s < 3; s++) {
                printf("%s:\n", status_name(s));
                for (Task *t = head; t; t = t->next)
                    if (t->status == s) printf("  %s %s\n", t->title, t->note);
            }
        }
    }
    while (head) {
        Task *next = head->next;
        free(head->title); free(head->note); free(head);
        head = next;
    }
    return 0;
}