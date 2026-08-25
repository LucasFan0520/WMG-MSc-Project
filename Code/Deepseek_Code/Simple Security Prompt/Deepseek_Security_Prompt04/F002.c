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

void add_message(char *tag, char *msg) {
    Message *m = malloc(sizeof(Message));
    m->tag = strdup(tag);
    m->msg = strdup(msg);
    m->next = NULL;
    if (!tail) head = tail = m;
    else { tail->next = m; tail = m; }
}

void concat_messages(char *tag1, char *tag2, char *newtag) {
    char *first = NULL, *second = NULL;
    for (Message *m = head; m; m = m->next) {
        if (strcmp(m->tag, tag1) == 0 && !first) first = m->msg;
        if (strcmp(m->tag, tag2) == 0 && !second) second = m->msg;
        if (first && second) break;
    }
    if (first && second) {
        char *combined = malloc(strlen(first) + 1 + strlen(second) + 1);
        sprintf(combined, "%s %s", first, second);
        add_message(newtag, combined);
        free(combined);
    }
}

void remove_tag(char *tag) {
    Message *prev = NULL;
    for (Message *m = head; m; ) {
        if (strcmp(m->tag, tag) == 0) {
            Message *next = m->next;
            if (prev) prev->next = next;
            else head = next;
            if (m == tail) tail = prev;
            free(m->tag);
            free(m->msg);
            free(m);
            m = next;
        } else {
            prev = m;
            m = m->next;
        }
    }
}

void find_first(char *tag) {
    for (Message *m = head; m; m = m->next) {
        if (strcmp(m->tag, tag) == 0) {
            for (char *c = m->msg; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            return;
        }
    }
}

void report_all(void) {
    for (Message *m = head; m; m = m->next) {
        printf("%s %s\n", m->tag, m->msg);
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256];
        int n = sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_message(arg1, arg2);
        } else if (strcmp(cmd, "CONCAT") == 0 && n == 4) {
            concat_messages(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_first(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}