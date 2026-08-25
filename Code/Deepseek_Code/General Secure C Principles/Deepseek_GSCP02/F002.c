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

void add_message(const char *tag, const char *msg) {
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->tag = strdup(tag);
    m->msg = strdup(msg);
    m->next = NULL;
    if (tail) {
        tail->next = m;
        tail = m;
    } else {
        head = tail = m;
    }
}

void remove_tag(const char *tag) {
    Message *prev = NULL;
    Message *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) {
            Message *tofree = m;
            if (prev) prev->next = m->next;
            else head = m->next;
            if (m == tail) tail = prev;
            m = m->next;
            free(tofree->tag);
            free(tofree->msg);
            free(tofree);
            continue;
        }
        prev = m;
        m = m->next;
    }
}

char *first_message(const char *tag) {
    Message *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) return m->msg;
        m = m->next;
    }
    return NULL;
}

void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    char *msg1 = first_message(tag1);
    char *msg2 = first_message(tag2);
    if (!msg1 || !msg2) return;
    size_t len = strlen(msg1) + strlen(msg2) + 2;
    char *combined = malloc(len);
    if (!combined) return;
    strcpy(combined, msg1);
    strcat(combined, " ");
    strcat(combined, msg2);
    add_message(newtag, combined);
    free(combined);
}

void find_message(const char *tag) {
    char *msg = first_message(tag);
    if (!msg) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *p = msg; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void report_all(void) {
    Message *m = head;
    while (m) {
        printf("%s ", m->tag);
        for (char *p = m->msg; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all(void) {
    while (head) {
        Message *next = head->next;
        free(head->tag);
        free(head->msg);
        free(head);
        head = next;
    }
    tail = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char tag[256], msg[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", tag, msg) == 2) {
                add_message(tag, msg);
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char tag1[256], tag2[256], newtag[256];
            if (sscanf(line, "%*s %255s %255s %255s", tag1, tag2, newtag) == 3) {
                concat_messages(tag1, tag2, newtag);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256];
            if (sscanf(line, "%*s %255s", tag) == 1) {
                remove_tag(tag);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256];
            if (sscanf(line, "%*s %255s", tag) == 1) {
                find_message(tag);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    free(line);
    free_all();
    return 0;
}