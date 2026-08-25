/* F002.c */
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

Message* find_first_by_tag(const char *tag) {
    Message *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(const char *tag, const char *msg) {
    Message *m = malloc(sizeof(Message));
    m->tag = strdup(tag);
    m->msg = strdup(msg);
    m->next = NULL;
    if (tail) tail->next = m;
    else head = m;
    tail = m;
}

void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    Message *m1 = find_first_by_tag(tag1);
    Message *m2 = find_first_by_tag(tag2);
    if (!m1 || !m2) return;
    char *combined = malloc(strlen(m1->msg) + strlen(m2->msg) + 2);
    strcpy(combined, m1->msg);
    strcat(combined, " ");
    strcat(combined, m2->msg);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Message *m = head, *prev = NULL;
    while (m) {
        if (strcmp(m->tag, tag) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            if (m == tail) tail = prev;
            free(m->tag);
            free(m->msg);
            free(m);
            m = prev ? prev->next : head;
            continue;
        }
        prev = m;
        m = m->next;
    }
}

void find_tag(const char *tag) {
    Message *m = find_first_by_tag(tag);
    if (!m) return;
    for (char *c = m->msg; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report() {
    Message *m = head;
    while (m) {
        printf("%s %s\n", m->tag, m->msg);
        m = m->next;
    }
}

void free_all() {
    while (head) {
        Message *tmp = head;
        head = head->next;
        free(tmp->tag);
        free(tmp->msg);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char tag[256]; char tag2[256]; char newtag[256]; char msg[4096];
        int n = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, tag, tag2, newtag, msg);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            // ADD tag message
            char message[4096];
            sscanf(line, "%*s %255s %[^\n]", tag, message);
            add_message(tag, message);
        } else if (strcmp(cmd, "CONCAT") == 0 && n == 5) {
            concat_messages(tag, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report();
        }
    }
    free_all();
    return 0;
}