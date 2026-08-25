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

Message* find_by_subject(const char *subject) {
    Message *m = head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void free_tags(TagNode *t) {
    while (t) {
        TagNode *tmp = t;
        t = t->next;
        free(tmp->tag);
        free(tmp);
    }
}

void free_message(Message *m) {
    free(m->sender);
    free(m->subject);
    free(m->body);
    free_tags(m->tags);
    free(m);
}

void handle_mail(char *cmd) {
    char *sender = strtok(NULL, " ");
    if (!sender) return;
    char *subject = strtok(NULL, " ");
    if (!subject) return;
    char *body = strtok(NULL, "");
    if (!body) body = "";
    if (find_by_subject(subject)) return; // we only keep first matching subject? spec says first matching for tag/untag/delete
    Message *new = malloc(sizeof(Message));
    new->sender = strdup(sender);
    new->subject = strdup(subject);
    new->body = strdup(body);
    new->tags = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Message *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_tag(char *cmd) {
    char *subject = strtok(NULL, " ");
    if (!subject) return;
    Message *m = find_by_subject(subject);
    if (!m) return;
    char *tag = strtok(NULL, " ");
    if (!tag) return;
    TagNode *t = m->tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return; // avoid duplicate
        t = t->next;
    }
    TagNode *new = malloc(sizeof(TagNode));
    new->tag = strdup(tag);
    new->next = NULL;
    if (!m->tags) m->tags = new;
    else {
        TagNode *last = m->tags;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_untag(char *cmd) {
    char *subject = strtok(NULL, " ");
    if (!subject) return;
    Message *m = find_by_subject(subject);
    if (!m) return;
    char *tag = strtok(NULL, " ");
    if (!tag) return;
    TagNode *t = m->tags, *prev = NULL;
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

void handle_delete(char *cmd) {
    char *subject = strtok(NULL, " ");
    if (!subject) return;
    Message *m = head, *prev = NULL;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            free_message(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void handle_search(char *cmd) {
    char *tag = strtok(NULL, " ");
    if (!tag) return;
    Message *m = head;
    while (m) {
        TagNode *t = m->tags;
        while (t) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void handle_report() {
    Message *m = head;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        TagNode *t = m->tags;
        while (t) {
            printf(" %s", t->tag);
            t = t->next;
        }
        putchar('\n');
        m = m->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "MAIL") == 0) handle_mail(cmd);
        else if (strcmp(cmd, "TAG") == 0) handle_tag(cmd);
        else if (strcmp(cmd, "UNTAG") == 0) handle_untag(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "SEARCH") == 0) handle_search(cmd);
        else if (strcmp(cmd, "REPORT") == 0) handle_report();
    }
    while (head) {
        Message *tmp = head;
        head = head->next;
        free_message(tmp);
    }
    return 0;
}