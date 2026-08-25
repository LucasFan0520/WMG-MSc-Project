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
    int tag_count;
    int tag_cap;
} Message;

Message *mailbox = NULL;
int msg_count = 0;
int msg_cap = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "MAIL") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *subj = space2 + 1;
                    char *space3 = strchr(subj, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *body = space3 + 1;
                        if (msg_count >= msg_cap) {
                            msg_cap = msg_cap == 0 ? 4 : msg_cap * 2;
                            Message *tmp = realloc(mailbox, msg_cap * sizeof(Message));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            mailbox = tmp;
                        }
                        mailbox[msg_count].sender = strdup(arg1);
                        mailbox[msg_count].subject = strdup(subj);
                        mailbox[msg_count].body = strdup(body);
                        mailbox[msg_count].tags = NULL;
                        mailbox[msg_count].tag_count = 0;
                        mailbox[msg_count].tag_cap = 0;
                        if (mailbox[msg_count].sender && mailbox[msg_count].subject && mailbox[msg_count].body) {
                            msg_count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "TAG") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *tag_name = space2 + 1;
                    for (int i = 0; i < msg_count; i++) {
                        if (strcmp(mailbox[i].subject, arg1) == 0) {
                            Message *m = &mailbox[i];
                            if (m->tag_count >= m->tag_cap) {
                                m->tag_cap = m->tag_cap == 0 ? 4 : m->tag_cap * 2;
                                char **tmp = realloc(m->tags, m->tag_cap * sizeof(char*));
                                if (!tmp) {
                                    free(line);
                                    return 1;
                                }
                                m->tags = tmp;
                            }
                            m->tags[m->tag_count] = strdup(tag_name);
                            if (m->tags[m->tag_count]) {
                                m->tag_count++;
                            }
                            break;
                        }
                    }
                }
            } else if (strcmp(cmd, "UNTAG") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *tag_name = space2 + 1;
                    for (int i = 0; i < msg_count; i++) {
                        if (strcmp(mailbox[i].subject, arg1) == 0) {
                            Message *m = &mailbox[i];
                            for (int j = 0; j < m->tag_count; j++) {
                                if (strcmp(m->tags[j], tag_name) == 0) {
                                    free(m->tags[j]);
                                    for (int k = j; k < m->tag_count - 1; k++) {
                                        m->tags[k] = m->tags[k + 1];
                                    }
                                    m->tag_count--;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                for (int i = 0; i < msg_count; i++) {
                    if (strcmp(mailbox[i].subject, arg1) == 0) {
                        free(mailbox[i].sender);
                        free(mailbox[i].subject);
                        free(mailbox[i].body);
                        for (int j = 0; j < mailbox[i].tag_count; j++) {
                            free(mailbox[i].tags[j]);
                        }
                        free(mailbox[i].tags);
                        for (int j = i; j < msg_count - 1; j++) {
                            mailbox[j] = mailbox[j + 1];
                        }
                        msg_count--;
                        break;
                    }
                }
            } else if (strcmp(cmd, "SEARCH") == 0) {
                for (int i = 0; i < msg_count; i++) {
                    int found = 0;
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        if (strcmp(mailbox[i].tags[j], arg1) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    if (found) {
                        printf("%s\n", mailbox[i].subject);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < msg_count; i++) {
                    printf("From: %s, Subj: %s, Body: %s, Tags:", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        printf(" %s", mailbox[i].tags[j]);
                    }
                    printf("\n");
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < msg_count; i++) {
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