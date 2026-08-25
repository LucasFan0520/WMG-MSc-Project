// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char *tag;
    char *msg;
    struct Message *next;
} Message;

Message *head = NULL, *tail = NULL;

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
    if (tail) {
        tail->next = m;
        tail = m;
    } else {
        head = tail = m;
    }
}

void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    Message *m1 = find_first(tag1);
    Message *m2 = find_first(tag2);
    if (!m1 || !m2) return;
    char *combined = malloc(strlen(m1->msg) + strlen(m2->msg) + 2);
    sprintf(combined, "%s %s", m1->msg, m2->msg);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Message *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->tag, tag) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Message *next = curr->next;
            free(curr->tag);
            free(curr->msg);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_and_print(const char *tag) {
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
        printf("%s %s\n", m->tag, m->msg);
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = strtok(NULL, " ");
            char *msg = strtok(NULL, "");
            if (tag && msg) add_message(tag, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = strtok(NULL, " ");
            char *tag2 = strtok(NULL, " ");
            char *newtag = strtok(NULL, " ");
            if (tag1 && tag2 && newtag) concat_messages(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) find_and_print(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}