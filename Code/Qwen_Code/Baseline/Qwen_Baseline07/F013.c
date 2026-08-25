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

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main() {
    char cmd[20], title[256], note[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "TODO") == 0) {
            scanf("%s", title); read_rest(note);
            Task *t = malloc(sizeof(Task));
            t->title = strdup(title); t->note = strdup(note); t->status = 0; t->next = head; head = t;
        } else if (strcmp(cmd, "START") == 0) {
            scanf("%s", title);
            for (Task *t = head; t; t = t->next) if (strcmp(t->title, title) == 0) { t->status = 1; break; }
        } else if (strcmp(cmd, "DONE") == 0) {
            scanf("%s", title);
            for (Task *t = head; t; t = t->next) if (strcmp(t->title, title) == 0) { t->status = 2; break; }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", title);
            Task *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->title); free(curr->note); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            for (Task *t = head; t; t = t->next)
                if (strcmp(t->title, title) == 0) {
                    const char *s = t->status == 0 ? "todo" : (t->status == 1 ? "doing" : "done");
                    printf("%s %s\n", s, t->note); break;
                }
        } else if (strcmp(cmd, "REPORT") == 0) {
            const char *names[] = {"todo", "doing", "done"};
            for (int i = 0; i < 3; i++) {
                printf("%s:", names[i]);
                for (Task *t = head; t; t = t->next)
                    if (t->status == i) printf(" %s", t->title);
                printf("\n");
            }
        }
    }
    return 0;
}