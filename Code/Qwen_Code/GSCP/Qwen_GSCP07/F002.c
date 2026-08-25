// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Msg { char *tag, *msg; struct Msg *next; } Msg;
Msg *head = NULL;

void add_msg(const char *tag, const char *msg) {
    Msg *m = malloc(sizeof(Msg));
    m->tag = strdup(tag);
    m->msg = strdup(msg);
    m->next = head;
    head = m;
}

void concat_msg(const char *t1, const char *t2, const char *nt) {
    Msg *m1 = NULL, *m2 = NULL;
    for (Msg *m = head; m; m = m->next) {
        if (!m1 && strcmp(m->tag, t1) == 0) m1 = m;
        if (!m2 && strcmp(m->tag, t2) == 0) m2 = m;
    }
    if (m1 && m2) {
        size_t len = strlen(m1->msg) + strlen(m2->msg) + 2;
        char *buf = malloc(len);
        snprintf(buf, len, "%s %s", m1->msg, m2->msg);
        add_msg(nt, buf);
        free(buf);
    }
}

void remove_tag(const char *tag) {
    Msg **p = &head;
    while (*p) {
        if (strcmp((*p)->tag, tag) == 0) {
            Msg *tmp = *p;
            *p = tmp->next;
            free(tmp->tag);
            free(tmp->msg);
            free(tmp);
        } else {
            p = &(*p)->next;
        }
    }
}

void find_msg(const char *tag) {
    for (Msg *m = head; m; m = m->next) {
        if (strcmp(m->tag, tag) == 0) {
            for (char *c = m->msg; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            return;
        }
    }
}

void report() {
    for (Msg *m = head; m; m = m->next) printf("%s %s\n", m->tag, m->msg);
}

void cleanup() {
    while (head) {
        Msg *tmp = head->next;
        free(head->tag);
        free(head->msg);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) add_msg(a1, t + 1); }
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", a1, a2, a3) == 3) concat_msg(a1, a2, a3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) remove_tag(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) find_msg(a1);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    cleanup();
    return 0;
}