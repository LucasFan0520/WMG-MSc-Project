// F023.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t tag_count;
    size_t tag_capacity;
} Mail;

int main(void) {
    Mail *mailbox = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *args = line + 5;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *sender = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *subject = rest;
                    char *body = sp2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Mail *next = realloc(mailbox, capacity * sizeof(Mail));
                        if (!next) break;
                        mailbox = next;
                    }
                    mailbox[count].sender = strdup(sender);
                    mailbox[count].subject = strdup(subject);
                    mailbox[count].body = strdup(body);
                    mailbox[count].tags = NULL;
                    mailbox[count].tag_count = 0;
                    mailbox[count].tag_capacity = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *subject = args;
                char *tag = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        if (mailbox[i].tag_count >= mailbox[i].tag_capacity) {
                            mailbox[i].tag_capacity = mailbox[i].tag_capacity == 0 ? 4 : mailbox[i].tag_capacity * 2;
                            char **nxt = realloc(mailbox[i].tags, mailbox[i].tag_capacity * sizeof(char *));
                            if (!nxt) break;
                            mailbox[i].tags = nxt;
                        }
                        mailbox[i].tags[mailbox[i].tag_count++] = strdup(tag);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *subject = args;
                char *tag = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        for (size_t k = 0; k < mailbox[i].tag_count; k++) {
                            if (strcmp(mailbox[i].tags[k], tag) == 0) {
                                free(mailbox[i].tags[k]);
                                for (size_t j = k; j < mailbox[i].tag_count - 1; j++) {
                                    mailbox[i].tags[j] = mailbox[i].tags[j + 1];
                                }
                                mailbox[i].tag_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subject) == 0) {
                    free(mailbox[i].sender);
                    free(mailbox[i].subject);
                    free(mailbox[i].body);
                    for (size_t k = 0; k < mailbox[i].tag_count; k++) {
                        free(mailbox[i].tags[k]);
                    }
                    free(mailbox[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        mailbox[j] = mailbox[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            for (size_t i = 0; i < count; i++) {
                int has_tag = 0;
                for (size_t k = 0; k < mailbox[i].tag_count; k++) {
                    if (strcmp(mailbox[i].tags[k], tag) == 0) {
                        has_tag = 1;
                        break;
                    }
                }
                if (has_tag) {
                    printf("%s\n", mailbox[i].subject);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(mailbox[i].sender);
        free(mailbox[i].subject);
        free(mailbox[i].body);
        for (size_t k = 0; k < mailbox[i].tag_count; k++) {
            free(mailbox[i].tags[k]);
        }
        free(mailbox[i].tags);
    }
    free(mailbox);
    free(line);
    return 0;
}
