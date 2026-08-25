// F023.c
#define _GNU_SOURCE
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
} Message;

static Message *messages = NULL;
static size_t msg_count = 0;
static size_t msg_cap = 0;

static ssize_t find_message_by_subject(const char *subject) {
    for (size_t i = 0; i < msg_count; i++)
        if (strcmp(messages[i].subject, subject) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_tag_to_msg(Message *m, const char *tag) {
    TagNode *cur = m->tags;
    while (cur) {
        if (strcmp(cur->tag, tag) == 0) return;
        cur = cur->next;
    }
    TagNode *new = malloc(sizeof(TagNode));
    if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
    new->tag = strdup(tag);
    if (!new->tag) { fprintf(stderr, "Memory error\n"); exit(1); }
    new->next = m->tags;
    m->tags = new;
}

static void remove_tag_from_msg(Message *m, const char *tag) {
    TagNode *prev = NULL, *cur = m->tags;
    while (cur) {
        if (strcmp(cur->tag, tag) == 0) {
            if (prev) prev->next = cur->next;
            else m->tags = cur->next;
            free(cur->tag);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

static void free_tags(Message *m) {
    TagNode *cur = m->tags;
    while (cur) {
        TagNode *next = cur->next;
        free(cur->tag);
        free(cur);
        cur = next;
    }
    m->tags = NULL;
}

static void add_message(const char *sender, const char *subject, const char *body) {
    if (find_message_by_subject(subject) != -1) return;
    if (msg_count == msg_cap) {
        msg_cap = msg_cap ? msg_cap * 2 : 4;
        Message *new = realloc(messages, msg_cap * sizeof(Message));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        messages = new;
    }
    messages[msg_count].sender = strdup(sender);
    messages[msg_count].subject = strdup(subject);
    messages[msg_count].body = strdup(body);
    messages[msg_count].tags = NULL;
    if (!messages[msg_count].sender || !messages[msg_count].subject || !messages[msg_count].body)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    msg_count++;
}

static void tag_subject(const char *subject, const char *tag) {
    ssize_t idx = find_message_by_subject(subject);
    if (idx == -1) return;
    add_tag_to_msg(&messages[idx], tag);
}

static void untag_subject(const char *subject, const char *tag) {
    ssize_t idx = find_message_by_subject(subject);
    if (idx == -1) return;
    remove_tag_from_msg(&messages[idx], tag);
}

static void delete_subject(const char *subject) {
    ssize_t idx = find_message_by_subject(subject);
    if (idx == -1) return;
    free(messages[idx].sender);
    free(messages[idx].subject);
    free(messages[idx].body);
    free_tags(&messages[idx]);
    for (size_t i = idx; i < msg_count - 1; i++)
        messages[i] = messages[i + 1];
    msg_count--;
}

static void search_tag(const char *tag) {
    for (size_t i = 0; i < msg_count; i++) {
        TagNode *cur = messages[i].tags;
        int found = 0;
        while (cur) {
            if (strcmp(cur->tag, tag) == 0) { found = 1; break; }
            cur = cur->next;
        }
        if (found)
            printf("%s\n", messages[i].subject);
    }
}

static void report_messages(void) {
    for (size_t i = 0; i < msg_count; i++) {
        printf("%s %s %s", messages[i].sender, messages[i].subject, messages[i].body);
        TagNode *cur = messages[i].tags;
        while (cur) {
            printf(" %s", cur->tag);
            cur = cur->next;
        }
        putchar('\n');
    }
}

static void free_all(void) {
    for (size_t i = 0; i < msg_count; i++) {
        free(messages[i].sender);
        free(messages[i].subject);
        free(messages[i].body);
        free_tags(&messages[i]);
    }
    free(messages);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], sender[256], subject[256], body[4096], tag[256];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "MAIL") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", sender, subject, body) == 3)
                add_message(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", subject, tag) == 2)
                tag_subject(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", subject, tag) == 2)
                untag_subject(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", subject) == 1)
                delete_subject(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (sscanf(line, "%*s %255s", tag) == 1)
                search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }

    free(line);
    free_all();
    return 0;
}