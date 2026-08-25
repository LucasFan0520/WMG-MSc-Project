// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Msg {
    char *tag;
    char *msg;
    struct Msg *next;
} Msg;

Msg *head = NULL;

void add_msg(const char *tag, const char *msg) {
    Msg *m = malloc(sizeof(Msg));
    m->tag = strdup(tag);
    m->msg = strdup(msg);
    m->next = head;
    head = m;
}

void concat_msg(const char *t1, const char *t2, const char *nt) {
    Msg *m1 = NULL;
    Msg *m2 = NULL;
    for (Msg *m = head; m; m = m->next) {
        if (!m1 && strcmp(m->tag, t1) == 0) {
            m1 = m;
        }
        if (!m2 && strcmp(m->tag, t2) == 0) {
            m2 = m;
        }
    }
    if (m1 && m2) {
        char *buf = malloc(strlen(m1->msg) + strlen(m2->msg) + 2);
        sprintf(buf, "%s %s", m1->msg, m2->msg);
        add_msg(nt, buf);
        free(buf);
    }
}

void remove_msg(const char *tag) {
    Msg *p = head;
    Msg *prev = NULL;
    while (p) {
        if (strcmp(p->tag, tag) == 0) {
            Msg *next = p->next;
            free(p->tag);
            free(p->msg);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            p = next;
        } else {
            prev = p;
            p = p->next;
        }
    }
}

void find_msg(const char *tag) {
    for (Msg *m = head; m; m = m->next) {
        if (strcmp(m->tag, tag) == 0) {
            for (char *c = m->msg; *c; c++) {
                if (*c == ' ') {
                    putchar('_');
                } else {
                    putchar(*c);
                }
            }
            putchar('\n');
            return;
        }
    }
}

void report_msg() {
    for (Msg *m = head; m; m = m->next) {
        printf("%s %s\n", m->tag, m->msg);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char a[1000];
        char b[1000];
        char c[1000];
        if (sscanf(line, "%s", cmd) != 1) {
            continue;
        }
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %[^\\n]", a, b);
            add_msg(a, b);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            sscanf(line, "%*s %s %s %s", a, b, c);
            concat_msg(a, b, c);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", a);
            remove_msg(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            find_msg(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_msg();
        }
    }
    while (head) {
        Msg *next = head->next;
        free(head->tag);
        free(head->msg);
        free(head);
        head = next;
    }
    return 0;
}