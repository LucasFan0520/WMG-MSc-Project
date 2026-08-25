// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char *tag;
    char *msg;
    struct Message *next;
} Message;

Message *head = NULL;
Message *tail = NULL;

Message *find_first(const char *tag) {
    Message *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(const char *tag, const char *msg) {
    Message *m = malloc(sizeof(Message));
    m->tag = malloc(strlen(tag) + 1);
    strcpy(m->tag, tag);
    m->msg = malloc(strlen(msg) + 1);
    strcpy(m->msg, msg);
    m->next = NULL;
    if (tail) tail->next = m;
    else head = m;
    tail = m;
}

void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    Message *m1 = find_first(tag1);
    Message *m2 = find_first(tag2);
    if (!m1 || !m2) return;
    char *combined = malloc(strlen(m1->msg) + strlen(m2->msg) + 2);
    strcpy(combined, m1->msg);
    strcat(combined, " ");
    strcat(combined, m2->msg);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Message *prev = NULL;
    Message *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) {
            Message *next = m->next;
            if (prev) prev->next = next;
            else head = next;
            if (tail == m) tail = prev;
            free(m->tag);
            free(m->msg);
            free(m);
            m = next;
            continue;
        }
        prev = m;
        m = m->next;
    }
}

void find_tag(const char *tag) {
    Message *m = find_first(tag);
    if (!m) return;
    for (char *c = m->msg; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_all(void) {
    Message *m = head;
    while (m) {
        printf("%s:", m->tag);
        for (char *c = m->msg; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all(void) {
    Message *m = head;
    while (m) {
        Message *next = m->next;
        free(m->tag);
        free(m->msg);
        free(m);
        m = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_message(tag, p);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *tag2 = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            concat_messages(tag1, tag2, p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_tag(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_tag(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}