// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
} Mail;

int main() {
    Mail *mailbox = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = strtok(NULL, " ");
            char *subject = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (sender && subject && body) {
                mailbox = realloc(mailbox, (count + 1) * sizeof(Mail));
                mailbox[count].sender = strdup(sender);
                mailbox[count].subject = strdup(subject);
                mailbox[count].body = strdup(body);
                mailbox[count].tags = NULL;
                mailbox[count].tag_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = strtok(NULL, " ");
            char *tag = strtok(NULL, " ");
            if (subject && tag) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        mailbox[i].tags = realloc(mailbox[i].tags, (mailbox[i].tag_count + 1) * sizeof(char *));
                        mailbox[i].tags[mailbox[i].tag_count] = strdup(tag);
                        mailbox[i].tag_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = strtok(NULL, " ");
            char *tag = strtok(NULL, " ");
            if (subject && tag) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        for (int j = 0; j < mailbox[i].tag_count; j++) {
                            if (strcmp(mailbox[i].tags[j], tag) == 0) {
                                free(mailbox[i].tags[j]);
                                for (int k = j; k < mailbox[i].tag_count - 1; k++) {
                                    mailbox[i].tags[k] = mailbox[i].tags[k + 1];
                                }
                                mailbox[i].tag_count--;
                                if (mailbox[i].tag_count == 0) {
                                    free(mailbox[i].tags);
                                    mailbox[i].tags = NULL;
                                } else {
                                    mailbox[i].tags = realloc(mailbox[i].tags, mailbox[i].tag_count * sizeof(char *));
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = strtok(NULL, " ");
            if (subject) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        free(mailbox[i].sender);
                        free(mailbox[i].subject);
                        free(mailbox[i].body);
                        for (int j = 0; j < mailbox[i].tag_count; j++) {
                            free(mailbox[i].tags[j]);
                        }
                        free(mailbox[i].tags);
                        for (int j = i; j < count - 1; j++) {
                            mailbox[j] = mailbox[j + 1];
                        }
                        count--;
                        break;
                    }
                }
                if (count == 0) {
                    free(mailbox);
                    mailbox = NULL;
                } else {
                    mailbox = realloc(mailbox, count * sizeof(Mail));
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) {
                for (int i = 0; i < count; i++) {
                    int found = 0;
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        if (strcmp(mailbox[i].tags[j], tag) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    if (found) {
                        printf("%s\n", mailbox[i].subject);
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("From: %s, Subject: %s, Body: %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(mailbox[i].sender);
        free(mailbox[i].subject);
        free(mailbox[i].body);
        for (int j = 0; j < mailbox[i].tag_count; j++) {
            free(mailbox[i].tags[j]);
        }
        free(mailbox[i].tags);
    }
    free(mailbox);
    return 0;
}
