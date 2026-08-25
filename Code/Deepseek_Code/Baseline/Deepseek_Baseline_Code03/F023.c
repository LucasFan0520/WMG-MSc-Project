// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tag_node {
    char *tag;
    struct tag_node *next;
};

struct message {
    char *sender;
    char *subject;
    char *body;
    struct tag_node *tags;
    struct message *next;
};

struct message *messages = NULL;

struct message *find_by_subject(char *subject) {
    struct message *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(char *sender, char *subject, char *body) {
    struct message *m = malloc(sizeof(struct message));
    m->sender = malloc(strlen(sender) + 1);
    strcpy(m->sender, sender);
    m->subject = malloc(strlen(subject) + 1);
    strcpy(m->subject, subject);
    m->body = malloc(strlen(body) + 1);
    strcpy(m->body, body);
    m->tags = NULL;
    m->next = messages;
    messages = m;
}

void tag_message(char *subject, char *tag) {
    struct message *m = find_by_subject(subject);
    if (!m) return;
    struct tag_node *t = m->tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return;
        t = t->next;
    }
    t = malloc(sizeof(struct tag_node));
    t->tag = malloc(strlen(tag) + 1);
    strcpy(t->tag, tag);
    t->next = m->tags;
    m->tags = t;
}

void untag_message(char *subject, char *tag) {
    struct message *m = find_by_subject(subject);
    if (!m) return;
    struct tag_node *t = m->tags, *prev = NULL;
    while (t) {
        if (strcmp(t->tag, tag) == 0) {
            if (prev) prev->next = t->next;
            else m->tags = t->next;
            free(t->tag);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void delete_message(char *subject) {
    struct message *m = messages, *prev = NULL;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else messages = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            struct tag_node *t = m->tags;
            while (t) {
                struct tag_node *next = t->next;
                free(t->tag);
                free(t);
                t = next;
            }
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void search_tag(char *tag) {
    struct message *m = messages;
    int found = 0;
    while (m) {
        struct tag_node *t = m->tags;
        while (t) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                found = 1;
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void report_messages() {
    struct message *m = messages;
    while (m) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
        m = m->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        int n = sscanf(line, "%31s %255s %255s %4095[^\n]", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "MAIL") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_message(arg1, arg2, rest);
        } else if (strcmp(cmd, "TAG") == 0 && n >= 3) {
            tag_message(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0 && n >= 3) {
            untag_message(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_message(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0 && n >= 2) {
            search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    return 0;
}