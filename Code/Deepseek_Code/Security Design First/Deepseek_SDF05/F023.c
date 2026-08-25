// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Tag {
    char *name;
    struct Tag *next;
} Tag;

typedef struct Message {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    struct Message *next;
} Message;

Message *messages = NULL;

Message *find_message_by_subject(const char *subject) {
    Message *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

Tag *find_tag(Message *m, const char *name) {
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void mail_add(const char *sender, const char *subject, const char *body) {
    if (find_message_by_subject(subject)) return;
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->sender = safe_strdup(sender);
    m->subject = safe_strdup(subject);
    m->body = safe_strdup(body);
    m->tags = NULL;
    m->next = messages;
    messages = m;
}

void tag_add(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    if (find_tag(m, tag)) return;
    Tag *t = malloc(sizeof(Tag));
    if (!t) return;
    t->name = safe_strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void tag_remove(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL, *cur = m->tags;
    while (cur) {
        if (strcmp(cur->name, tag) == 0) {
            if (prev) prev->next = cur->next;
            else m->tags = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void message_delete(const char *subject) {
    Message *prev = NULL, *cur = messages;
    while (cur) {
        if (strcmp(cur->subject, subject) == 0) {
            if (prev) prev->next = cur->next;
            else messages = cur->next;
            free(cur->sender);
            free(cur->subject);
            free(cur->body);
            Tag *t = cur->tags;
            while (t) {
                Tag *next = t->next;
                free(t->name);
                free(t);
                t = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void search_tag(const char *tag) {
    Message *m = messages;
    int found = 0;
    while (m) {
        if (find_tag(m, tag)) {
            printf("%s\n", m->subject);
            found = 1;
        }
        m = m->next;
    }
    if (!found) printf("NOT_FOUND\n");
}

void report_messages(void) {
    Message *m = messages;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        Tag *t = m->tags;
        while (t) {
            printf(" %s", t->name);
            t = t->next;
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all(void) {
    Message *m = messages;
    while (m) {
        Message *next_m = m->next;
        free(m->sender);
        free(m->subject);
        free(m->body);
        Tag *t = m->tags;
        while (t) {
            Tag *next_t = t->next;
            free(t->name);
            free(t);
            t = next_t;
        }
        free(m);
        m = next_m;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "MAIL") == 0 && n >= 2) {
            char sender[512], subject[512];
            if (sscanf(line, "%*s %511s %511s", sender, subject) != 2) continue;
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            mail_add(sender, subject, p);
        } else if (strcmp(cmd, "TAG") == 0 && n >= 3) {
            tag_add(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0 && n >= 3) {
            tag_remove(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            message_delete(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0 && n >= 2) {
            search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    free_all();
    return 0;
}