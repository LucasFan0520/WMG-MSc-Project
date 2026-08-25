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
Msg *tail = NULL;

Msg *find_first(const char *tag) {
    Msg *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_msg(const char *tag, const char *msg) {
    Msg *m = malloc(sizeof(Msg));
    m->tag = malloc(strlen(tag)+1);
    strcpy(m->tag, tag);
    m->msg = malloc(strlen(msg)+1);
    strcpy(m->msg, msg);
    m->next = NULL;
    if (tail) tail->next = m;
    else head = m;
    tail = m;
}

void concat_msgs(const char *tag1, const char *tag2, const char *newtag) {
    Msg *m1 = find_first(tag1);
    Msg *m2 = find_first(tag2);
    if (!m1 || !m2) return;
    char *combined = malloc(strlen(m1->msg) + strlen(m2->msg) + 2);
    strcpy(combined, m1->msg);
    strcat(combined, " ");
    strcat(combined, m2->msg);
    add_msg(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Msg *prev = NULL;
    Msg *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            if (m == tail) tail = prev;
            free(m->tag);
            free(m->msg);
            free(m);
            m = prev ? prev->next : head;
        } else {
            prev = m;
            m = m->next;
        }
    }
    if (!head) tail = NULL;
}

void find_tag(const char *tag) {
    Msg *m = find_first(tag);
    if (!m) return;
    for (char *c = m->msg; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report_all(void) {
    Msg *m = head;
    while (m) {
        printf("%s %s\n", m->tag, m->msg);
        m = m->next;
    }
}

void free_all(void) {
    Msg *m = head;
    while (m) {
        Msg *next = m->next;
        free(m->tag);
        free(m->msg);
        free(m);
        m = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = strtok(rest, " ");
            if (!tag) continue;
            char *msg = strtok(NULL, "");
            if (!msg) msg = "";
            add_msg(tag, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = strtok(rest, " ");
            char *tag2 = strtok(NULL, " ");
            char *newtag = strtok(NULL, " ");
            if (tag1 && tag2 && newtag) concat_msgs(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = strtok(rest, " ");
            if (tag) remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = strtok(rest, " ");
            if (tag) find_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}