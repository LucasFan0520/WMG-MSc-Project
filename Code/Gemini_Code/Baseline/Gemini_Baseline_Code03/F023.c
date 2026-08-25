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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256];
            int s_len = 0, sub_len = 0;
            if (sscanf(ptr, "%255s%n", sender, &s_len) > 0) {
                char *ptr2 = ptr + s_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%255s%n", subject, &sub_len) > 0) {
                    char *body = ptr2 + sub_len;
                    while (*body == ' ') body++;
                    mailbox = realloc(mailbox, (count + 1) * sizeof(Message));
                    mailbox[count].sender = strdup(sender);
                    mailbox[count].subject = strdup(subject);
                    mailbox[count].body = strdup(body);
                    mailbox[count].tags = NULL;
                    mailbox[count].tag_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(ptr, "%255s %255s", subject, tag) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        int tc = mailbox[i].tag_count;
                        mailbox[i].tags = realloc(mailbox[i].tags, (tc + 1) * sizeof(char *));
                        mailbox[i].tags[tc] = strdup(tag);
                        mailbox[i].tag_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(ptr, "%255s %255s", subject, tag) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        int f_idx = -1;
                        for (int j = 0; j < mailbox[i].tag_count; j++) {
                            if (strcmp(mailbox[i].tags[j], tag) == 0) {
                                f_idx = j;
                                break;
                            }
                        }
                        if (f_idx != -1) {
                            free(mailbox[i].tags[f_idx]);
                            for (int j = f_idx; j < mailbox[i].tag_count - 1; j++) {
                                mailbox[i].tags[j] = mailbox[i].tags[j + 1];
                            }
                            mailbox[i].tag_count--;
                            if (mailbox[i].tag_count > 0) {
                                mailbox[i].tags = realloc(mailbox[i].tags, mailbox[i].tag_count * sizeof(char *));
                            } else {
                                free(mailbox[i].tags);
                                mailbox[i].tags = NULL;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[256];
            if (sscanf(ptr, "%255s", subject) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(mailbox[found].sender);
                    free(mailbox[found].subject);
                    free(mailbox[found].body);
                    for (int j = 0; j < mailbox[found].tag_count; j++) {
                        free(mailbox[found].tags[j]);
                    }
                    free(mailbox[found].tags);
                    for (int i = found; i < count - 1; i++) {
                        mailbox[i] = mailbox[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        mailbox = realloc(mailbox, count * sizeof(Message));
                    } else {
                        free(mailbox);
                        mailbox = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256];
            if (sscanf(ptr, "%255s", tag) > 0) {
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
