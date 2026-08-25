// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TAG 64
#define MAX_MSG 4096

typedef struct Message {
    char tag[MAX_TAG];
    char msg[MAX_MSG];
    struct Message *next;
} Message;

Message *head = NULL;

Message *find_first_message(const char *tag) {
    Message *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void delete_all_messages(const char *tag) {
    Message *m = head, *prev = NULL;
    while (m) {
        if (strcmp(m->tag, tag) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            free(m);
            m = prev ? prev->next : head;
        } else {
            prev = m;
            m = m->next;
        }
    }
}

void free_all() {
    Message *m = head;
    while (m) {
        Message *tmp = m;
        m = m->next;
        free(tmp);
    }
    head = NULL;
}

void handle_add(char *line) {
    char tag[MAX_TAG];
    char msg[MAX_MSG];
    int n = sscanf(line, "ADD %63s %4095[^\n]", tag, msg);
    if (n < 2) return;
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    strcpy(m->tag, tag);
    strcpy(m->msg, msg);
    m->next = head;
    head = m;
}

void handle_concat(char *line) {
    char tag1[MAX_TAG], tag2[MAX_TAG], newtag[MAX_TAG];
    if (sscanf(line, "CONCAT %63s %63s %63s", tag1, tag2, newtag) != 3) return;
    Message *m1 = find_first_message(tag1);
    Message *m2 = find_first_message(tag2);
    if (!m1 || !m2) return;
    char combined[MAX_MSG];
    int len = snprintf(combined, sizeof(combined), "%s %s", m1->msg, m2->msg);
    if (len < 0 || len >= MAX_MSG) return;
    Message *newm = malloc(sizeof(Message));
    if (!newm) return;
    strcpy(newm->tag, newtag);
    strcpy(newm->msg, combined);
    newm->next = head;
    head = newm;
}

void handle_remove(char *line) {
    char tag[MAX_TAG];
    if (sscanf(line, "REMOVE %63s", tag) != 1) return;
    delete_all_messages(tag);
}

void handle_find(char *line) {
    char tag[MAX_TAG];
    if (sscanf(line, "FIND %63s", tag) != 1) return;
    Message *m = find_first_message(tag);
    if (!m) return;
    for (char *c = m->msg; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void handle_report() {
    Message *m = head;
    while (m) {
        printf("%s %s\n", m->tag, m->msg);
        m = m->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "CONCAT ", 7) == 0) handle_concat(line);
        else if (strncmp(line, "REMOVE ", 7) == 0) handle_remove(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}