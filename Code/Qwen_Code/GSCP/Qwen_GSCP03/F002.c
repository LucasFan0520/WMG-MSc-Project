// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Msg {
    char *tag, *msg;
    struct Msg *next;
} Msg;

Msg *head = NULL;

void free_list() {
    while (head) {
        Msg *t = head; head = head->next;
        free(t->tag); free(t->msg); free(t);
    }
}

Msg *find_tag(const char *tag) {
    for (Msg *m = head; m; m = m->next)
        if (strcmp(m->tag, tag) == 0) return m;
    return NULL;
}

void print_spaces(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], t1[256], t2[256], t3[256], msg[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", t1, msg) < 1) continue;
            Msg *m = malloc(sizeof(Msg));
            m->tag = strdup(t1); m->msg = strdup(msg); m->next = head; head = m;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", t1, t2, t3) != 3) continue;
            Msg *m1 = find_tag(t1), *m2 = find_tag(t2);
            if (m1 && m2) {
                Msg *m = malloc(sizeof(Msg));
                m->tag = strdup(t3);
                m->msg = malloc(strlen(m1->msg) + strlen(m2->msg) + 2);
                sprintf(m->msg, "%s %s", m1->msg, m2->msg);
                m->next = head; head = m;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", t1) != 1) continue;
            Msg *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->tag, t1) == 0) {
                    Msg *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->tag); free(tmp->msg); free(tmp);
                } else { prev = curr; curr = curr->next; }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", t1) != 1) continue;
            Msg *m = find_tag(t1);
            if (m) print_spaces(m->msg);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Msg *m = head; m; m = m->next) printf("%s %s\n", m->tag, m->msg);
        }
    }
    free_list();
    return 0;
}