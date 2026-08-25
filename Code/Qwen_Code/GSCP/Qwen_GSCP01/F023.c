// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TagNode {
    char *tag;
    struct TagNode *next;
} TagNode;

typedef struct Mail {
    char *sender;
    char *subject;
    char *body;
    TagNode *tags;
    struct Mail *next;
} Mail;

Mail *mails = NULL;

void create_mail(const char *sender, const char *subject, const char *body) {
    Mail *m = malloc(sizeof(Mail));
    if (!m) exit(EXIT_FAILURE);
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = mails;
    mails = m;
}

void tag_mail(const char *subject, const char *tag) {
    Mail *curr = mails;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            TagNode *t = curr->tags;
            while (t) {
                if (strcmp(t->tag, tag) == 0) return;
                t = t->next;
            }
            t = malloc(sizeof(TagNode));
            if (!t) exit(EXIT_FAILURE);
            t->tag = strdup(tag);
            t->next = curr->tags;
            curr->tags = t;
            return;
        }
        curr = curr->next;
    }
}

void untag_mail(const char *subject, const char *tag) {
    Mail *curr = mails;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            TagNode *t_curr = curr->tags;
            TagNode *t_prev = NULL;
            while (t_curr) {
                if (strcmp(t_curr->tag, tag) == 0) {
                    if (t_prev) t_prev->next = t_curr->next;
                    else curr->tags = t_curr->next;
                    free(t_curr->tag);
                    free(t_curr);
                    return;
                }
                t_prev = t_curr;
                t_curr = t_curr->next;
            }
            return;
        }
        curr = curr->next;
    }
}

void delete_mail(const char *subject) {
    Mail *curr = mails;
    Mail *prev = NULL;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            if (prev) prev->next = curr->next;
            else mails = curr->next;
            TagNode *t = curr->tags;
            while (t) {
                TagNode *next_t = t->next;
                free(t->tag);
                free(t);
                t = next_t;
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

void search_mail(const char *tag) {
    Mail *curr = mails;
    while (curr) {
        TagNode *t = curr->tags;
        while (t) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", curr->subject);
                break;
            }
            t = t->next;
        }
        curr = curr->next;
    }
}

void report_mails() {
    Mail *curr = mails;
    while (curr) {
        printf("%s %s %s\n", curr->sender, curr->subject, curr->body);
        curr = curr->next;
    }
}

void free_all() {
    Mail *curr = mails;
    while (curr) {
        Mail *next_m = curr->next;
        TagNode *t = curr->tags;
        while (t) {
            TagNode *next_t = t->next;
            free(t->tag);
            free(t);
            t = next_t;
        }
        free(curr->sender);
        free(curr->subject);
        free(curr->body);
        free(curr);
        curr = next_m;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[3500] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        
        if (strcmp(cmd, "MAIL") == 0 && matched >= 3) {
            create_mail(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TAG") == 0 && matched >= 2) {
            tag_mail(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0 && matched >= 2) {
            untag_mail(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_mail(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            search_mail(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_mails();
        }
    }
    free_all();
    return 0;
}