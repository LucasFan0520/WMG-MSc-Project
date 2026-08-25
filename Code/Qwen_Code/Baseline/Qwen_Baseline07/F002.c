// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Msg {
    char *tag, *msg;
    struct Msg *next;
} Msg;

Msg *head = NULL, *tail = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

void print_under(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

void add_msg(const char *tag, const char *msg) {
    Msg *m = malloc(sizeof(Msg));
    m->tag = strdup(tag); m->msg = strdup(msg); m->next = NULL;
    if (!head) head = tail = m; else { tail->next = m; tail = m; }
}

Msg *find_first(const char *tag) {
    for (Msg *m = head; m; m = m->next) if (strcmp(m->tag, tag) == 0) return m;
    return NULL;
}

int main() {
    char cmd[20], t1[256], t2[256], t3[256], text[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", t1); read_rest(text); add_msg(t1, text);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            scanf("%s %s %s", t1, t2, t3);
            Msg *m1 = find_first(t1), *m2 = find_first(t2);
            if (m1 && m2) {
                char *new_msg = malloc(strlen(m1->msg) + strlen(m2->msg) + 2);
                sprintf(new_msg, "%s %s", m1->msg, m2->msg);
                add_msg(t3, new_msg);
                free(new_msg);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", t1);
            Msg *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->tag, t1) == 0) {
                    Msg *next = curr->next;
                    if (prev) prev->next = next; else head = next;
                    if (!next) tail = prev;
                    free(curr->tag); free(curr->msg); free(curr);
                    curr = next;
                } else { prev = curr; curr = curr->next; }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", t1);
            Msg *m = find_first(t1);
            if (m) print_under(m->msg);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Msg *m = head; m; m = m->next) printf("%s %s\n", m->tag, m->msg);
        }
    }
    return 0;
}