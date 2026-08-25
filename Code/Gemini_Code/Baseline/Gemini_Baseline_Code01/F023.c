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
} Message;

int main() {
    Message *mailbox = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256];
            int bytes = 0;
            if (sscanf(args, "%255s %255s%n", sender, subject, &bytes) == 2) {
                char *body = args + bytes;
                while (*body == ' ') body++;
                mailbox = realloc(mailbox, (count + 1) * sizeof(Message));
                mailbox[count].sender = strdup(sender);
                mailbox[count].subject = strdup(subject);
                mailbox[count].body = strdup(body);
                mailbox[count].tags = NULL;
                mailbox[count].tag_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(args, "%255s %255s", subject, tag) == 2) {
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
            char subject[256], tag[256];
            if (sscanf(args, "%255s %255s", subject, tag) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        for (int j = 0; j < mailbox[i].tag_count; j++) {
                            if (strcmp(mailbox[i].tags[j], tag) == 0) {
                                free(mailbox[i].tags[j]);
                                for (int k = j; k < mailbox[i].tag_count - 1; k++) {
                                    mailbox[i].tags[k] = mailbox[i].tags[k + 1];
                                }
                                mailbox[i].tag_count--;
                                mailbox[i].tags = realloc(mailbox[i].tags, mailbox[i].tag_count * sizeof(char *));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[256];
            if (sscanf(args, "%255s", subject) == 1) {
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
                        mailbox = realloc(mailbox, count * sizeof(Message));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256];
            if (sscanf(args, "%255s", tag) == 1) {
                for (int i = 0; i < count; i++) {
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        if (strcmp(mailbox[i].tags[j], tag) == 0) {
                            printf("%s\n", mailbox[i].subject);
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("From: %s Subject: %s Body: %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
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
