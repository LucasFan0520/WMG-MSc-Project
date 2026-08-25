// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Mail *head = NULL;

void add_mail(const char *sender, const char *subject, const char *body) {
    Mail *m = malloc(sizeof(Mail));
    if (!m) return;
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    if (!m->sender || !m->subject || !m->body) {
        free(m->sender); free(m->subject); free(m->body); free(m); return;
    }
    m->tags = NULL;
    m->next = head;
    head = m;
}

void tag_mail(const char *subject, const char *tag) {
    for (Mail *m = head; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            for (Tag *t = m->tags; t; t = t->next) {
                if (strcmp(t->name, tag) == 0) return;
            }
            Tag *t = malloc(sizeof(Tag));
            if (!t) return;
            t->name = strdup(tag);
            if (!t->name) { free(t); return; }
            t->next = m->tags;
            m->tags = t;
            return;
        }
    }
}

void untag_mail(const char *subject, const char *tag) {
    for (Mail *m = head; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            Tag *curr = m->tags;
            Tag *prev = NULL;
            while (curr) {
                if (strcmp(curr->name, tag) == 0) {
                    if (prev) prev->next = curr->next;
                    else m->tags = curr->next;
                    free(curr->name);
                    free(curr);
                    return;
                }
                prev = curr;
                curr = curr->next;
            }
            return;
        }
    }
}

void delete_mail(const char *subject) {
    Mail *curr = head;
    Mail *prev = NULL;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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

void search_tag(const char *tag) {
    for (Mail *m = head; m; m = m->next) {
        for (Tag *t = m->tags; t; t = t->next) {
            if (strcmp(t->name, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
        }
    }
}

void report() {
    for (Mail *m = head; m; m = m->next) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
        for (Tag *t = m->tags; t; t = t->next) {
            printf("TAG %s\n", t->name);
        }
    }
}

void free_all() {
    Mail *m = head;
    while (m) {
        Mail *next = m->next;
        Tag *t = m->tags;
        while (t) {
            Tag *next_t = t->next;
            free(t->name);
            free(t);
            t = next_t;
        }
        free(m->sender);
        free(m->subject);
        free(m->body);
        free(m);
        m = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[1024] = {0};
        char arg3[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %1023s %4095[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        
        if (strcmp(cmd, "MAIL") == 0 && n >= 4) {
            add_mail(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TAG") == 0 && n >= 3) {
            tag_mail(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0 && n >= 3) {
            untag_mail(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_mail(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0 && n >= 2) {
            search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}