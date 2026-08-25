// F023.c
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

Message *head = NULL;

Message* find_first_by_subject(const char *subject) {
    Message *cur = head;
    while (cur) {
        if (strcmp(cur->subject, subject) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_message(const char *sender, const char *subject, const char *body) {
    Message *new = malloc(sizeof(Message));
    new->sender = strdup(sender);
    new->subject = strdup(subject);
    new->body = strdup(body);
    new->tags = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Message *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void add_tag(const char *subject, const char *tag) {
    Message *m = find_first_by_subject(subject);
    if (!m) return;
    TagNode *cur = m->tags;
    while (cur) {
        if (strcmp(cur->tag, tag) == 0) return;
        cur = cur->next;
    }
    TagNode *new = malloc(sizeof(TagNode));
    new->tag = strdup(tag);
    new->next = NULL;
    if (!m->tags) m->tags = new;
    else {
        cur = m->tags;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void remove_tag(const char *subject, const char *tag) {
    Message *m = find_first_by_subject(subject);
    if (!m) return;
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

void delete_message(const char *subject) {
    Message *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->subject, subject) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->sender);
            free(cur->subject);
            free(cur->body);
            TagNode *t = cur->tags;
            while (t) {
                TagNode *tmp = t;
                t = t->next;
                free(tmp->tag);
                free(tmp);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void search_tag(const char *tag) {
    Message *cur = head;
    while (cur) {
        TagNode *t = cur->tags;
        while (t) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", cur->subject);
                break;
            }
            t = t->next;
        }
        cur = cur->next;
    }
}

void report() {
    Message *cur = head;
    while (cur) {
        printf("%s %s %s", cur->sender, cur->subject, cur->body);
        TagNode *t = cur->tags;
        while (t) {
            printf(" %s", t->tag);
            t = t->next;
        }
        printf("\n");
        cur = cur->next;
    }
}

void free_all() {
    Message *cur = head;
    while (cur) {
        Message *tmp = cur;
        cur = cur->next;
        free(tmp->sender);
        free(tmp->subject);
        free(tmp->body);
        TagNode *t = tmp->tags;
        while (t) {
            TagNode *tt = t;
            t = t->next;
            free(tt->tag);
            free(tt);
        }
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = strtok(NULL, " ");
            if (!sender) continue;
            char *subject = strtok(NULL, " ");
            if (!subject) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            add_message(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = strtok(NULL, " ");
            if (!subject) continue;
            char *tag = strtok(NULL, " ");
            if (tag) add_tag(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = strtok(NULL, " ");
            if (!subject) continue;
            char *tag = strtok(NULL, " ");
            if (tag) remove_tag(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = strtok(NULL, " ");
            if (subject) delete_message(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}