// filename: F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TagNode {
    char *tag;
    struct TagNode *next;
} TagNode;

typedef struct Message {
    char *sender;
    char *subject;
    char *body;
    TagNode *tags;
    struct Message *next;
} Message;

static Message *messages = NULL;

static void free_tags(TagNode *t) {
    while (t) {
        TagNode *tmp = t;
        t = t->next;
        free(tmp->tag);
        free(tmp);
    }
}

static void cleanup(void) {
    while (messages) {
        Message *tmp = messages;
        messages = messages->next;
        free(tmp->sender);
        free(tmp->subject);
        free(tmp->body);
        free_tags(tmp->tags);
        free(tmp);
    }
}

static Message *find_by_subject(const char *subject) {
    for (Message *m = messages; m; m = m->next)
        if (strcmp(m->subject, subject) == 0)
            return m;
    return NULL;
}

static void mail_add(const char *sender, const char *subject, const char *body) {
    if (find_by_subject(subject)) return; // only first matching? spec says "first matching subject" for TAG, but we allow multiple? Actually it says "first matching subject" for TAG and DELETE. So we allow duplicates? The spec: "MAIL sender subject body creates a message", "TAG subject tag adds a tag to the first matching subject", "UNTAG subject tag removes a tag from the first matching subject", "DELETE subject deletes the first matching message". So we need to allow multiple messages with same subject, and operations affect the first one found. We'll implement linked list, and find first by subject. For ADD, we allow duplicates.
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    if (!m->sender || !m->subject || !m->body) {
        free(m->sender);
        free(m->subject);
        free(m->body);
        free(m);
        return;
    }
    m->tags = NULL;
    m->next = messages;
    messages = m;
}

static Message *find_first_subject(const char *subject) {
    for (Message *m = messages; m; m = m->next)
        if (strcmp(m->subject, subject) == 0)
            return m;
    return NULL;
}

static void tag_add(const char *subject, const char *tag) {
    Message *m = find_first_subject(subject);
    if (!m) return;
    TagNode *tn = malloc(sizeof(TagNode));
    if (!tn) return;
    tn->tag = strdup(tag);
    if (!tn->tag) { free(tn); return; }
    tn->next = m->tags;
    m->tags = tn;
}

static void tag_remove(const char *subject, const char *tag) {
    Message *m = find_first_subject(subject);
    if (!m) return;
    TagNode *prev = NULL;
    for (TagNode *t = m->tags; t; t = t->next) {
        if (strcmp(t->tag, tag) == 0) {
            if (prev) prev->next = t->next;
            else m->tags = t->next;
            free(t->tag);
            free(t);
            return;
        }
        prev = t;
    }
}

static void delete_first_subject(const char *subject) {
    Message *prev = NULL;
    for (Message *m = messages; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else messages = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            free_tags(m->tags);
            free(m);
            return;
        }
        prev = m;
    }
}

static void search_tag(const char *tag) {
    for (Message *m = messages; m; m = m->next) {
        for (TagNode *t = m->tags; t; t = t->next) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
        }
    }
}

static void report(void) {
    for (Message *m = messages; m; m = m->next) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        for (TagNode *t = m->tags; t; t = t->next)
            printf(" %s", t->tag);
        putchar('\n');
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "MAIL") == 0) {
            if (*arg1 && *arg2) mail_add(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TAG") == 0) {
            if (*arg1 && *arg2) tag_add(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (*arg1 && *arg2) tag_remove(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_first_subject(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (*arg1) search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free(line);
    return 0;
}