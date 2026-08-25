// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Tag {
    char *name;
    struct Tag *next;
} Tag;

typedef struct Mail {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    struct Mail *next;
} Mail;

Mail *mails = NULL;

Mail *find_mail_by_subject(const char *subject) {
    for (Mail *m = mails; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) return m;
    }
    return NULL;
}

void add_mail(const char *sender, const char *subject, const char *body) {
    Mail *m = malloc(sizeof(Mail));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = mails;
    mails = m;
}

void add_tag(const char *subject, const char *tag_name) {
    Mail *m = find_mail_by_subject(subject);
    if (!m) return;
    Tag *t = malloc(sizeof(Tag));
    t->name = strdup(tag_name);
    t->next = m->tags;
    m->tags = t;
}

void remove_tag(const char *subject, const char *tag_name) {
    Mail *m = find_mail_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL, *curr = m->tags;
    while (curr) {
        if (strcmp(curr->name, tag_name) == 0) {
            if (prev) prev->next = curr->next;
            else m->tags = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_mail(const char *subject) {
    Mail *prev = NULL, *curr = mails;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            if (prev) prev->next = curr->next;
            else mails = curr->next;
            Tag *t = curr->tags;
            while (t) {
                Tag *next = t->next;
                free(t->name);
                free(t);
                t = next;
            }
            free(curr->sender);
            free(curr->subject);
            free(curr->body);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void search_tag(const char *tag_name) {
    for (Mail *m = mails; m; m = m->next) {
        for (Tag *t = m->tags; t; t = t->next) {
            if (strcmp(t->name, tag_name) == 0) {
                printf("%s\n", m->subject);
                break;
            }
        }
    }
}

void report_mails() {
    for (Mail *m = mails; m; m = m->next) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        
        if (strcmp(cmd, "MAIL") == 0) {
            char *body = get_rest(line, 3);
            if (body) add_mail(arg1, arg2, body);
        }
        else if (strcmp(cmd, "TAG") == 0) add_tag(arg1, arg2);
        else if (strcmp(cmd, "UNTAG") == 0) remove_tag(arg1, arg2);
        else if (strcmp(cmd, "DELETE") == 0) delete_mail(arg1);
        else if (strcmp(cmd, "SEARCH") == 0) search_tag(arg1);
        else if (strcmp(cmd, "REPORT") == 0) report_mails();
        
        free(line);
    }
    return 0;
}