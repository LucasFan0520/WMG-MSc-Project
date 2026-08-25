// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char *tag;
    char *message;
    struct Message *next;
} Message;

Message *head = NULL;

Message *find_message_by_tag(const char *tag) {
    Message *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(const char *tag, const char *msg) {
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->tag = strdup(tag);
    m->message = strdup(msg);
    m->next = NULL;
    if (!m->tag || !m->message) {
        free(m->tag); free(m->message); free(m);
        return;
    }
    if (!head) head = m;
    else {
        Message *last = head;
        while (last->next) last = last->next;
        last->next = m;
    }
}

void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    Message *m1 = find_message_by_tag(tag1);
    Message *m2 = find_message_by_tag(tag2);
    if (!m1 || !m2) return;
    size_t len = strlen(m1->message) + 1 + strlen(m2->message) + 1;
    char *combined = malloc(len);
    if (!combined) return;
    snprintf(combined, len, "%s %s", m1->message, m2->message);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Message *m = head, *prev = NULL;
    while (m) {
        if (strcmp(m->tag, tag) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            free(m->tag);
            free(m->message);
            free(m);
            m = prev ? prev->next : head;
        } else {
            prev = m;
            m = m->next;
        }
    }
}

void find_tag(const char *tag) {
    Message *m = find_message_by_tag(tag);
    if (!m) return;
    for (char *c = m->message; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_all(void) {
    Message *m = head;
    while (m) {
        printf("%s %s\n", m->tag, m->message);
        m = m->next;
    }
}

void free_all(void) {
    Message *m = head;
    while (m) {
        Message *next = m->next;
        free(m->tag);
        free(m->message);
        free(m);
        m = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], tag1[256], tag2[256], newtag[256], msg[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", tag1, msg) == 2)
                add_message(tag1, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", tag1, tag2, newtag) == 3)
                concat_messages(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", tag1) == 1)
                remove_tag(tag1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", tag1) == 1)
                find_tag(tag1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}