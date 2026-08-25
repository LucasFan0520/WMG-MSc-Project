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
    int tag_capacity;
} Message;

Message *mailbox = NULL;
int m_count = 0;
int m_capacity = 0;

int find_by_subject(const char *sub) {
    for (int i = 0; i < m_count; i++) {
        if (strcmp(mailbox[i].subject, sub) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *p1 = line + 5;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *body = sp2 + 1;
                    if (m_count >= m_capacity) {
                        m_capacity = m_capacity == 0 ? 4 : m_capacity * 2;
                        mailbox = realloc(mailbox, m_capacity * sizeof(Message));
                    }
                    mailbox[m_count].sender = strdup(p1);
                    mailbox[m_count].subject = strdup(p2);
                    mailbox[m_count].body = strdup(body);
                    mailbox[m_count].tags = NULL;
                    mailbox[m_count].tag_count = 0;
                    mailbox[m_count].tag_capacity = 0;
                    m_count++;
                }
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p1 = line + 4;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *tag = sp + 1;
                int idx = find_by_subject(p1);
                if (idx != -1) {
                    Message *msg = &mailbox[idx];
                    if (msg->tag_count >= msg->tag_capacity) {
                        msg->tag_capacity = msg->tag_capacity == 0 ? 4 : msg->tag_capacity * 2;
                        msg->tags = realloc(msg->tags, msg->tag_capacity * sizeof(char *));
                    }
                    msg->tags[msg->tag_count] = strdup(tag);
                    msg->tag_count++;
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *p1 = line + 6;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *tag = sp + 1;
                int idx = find_by_subject(p1);
                if (idx != -1) {
                    Message *msg = &mailbox[idx];
                    for (int i = 0; i < msg->tag_count; i++) {
                        if (strcmp(msg->tags[i], tag) == 0) {
                            free(msg->tags[i]);
                            for (int j = i; j < msg->tag_count - 1; j++) {
                                msg->tags[j] = msg->tags[j + 1];
                            }
                            msg->tag_count--;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *sub = line + 7;
            int idx = find_by_subject(sub);
            if (idx != -1) {
                free(mailbox[idx].sender);
                free(mailbox[idx].subject);
                free(mailbox[idx].body);
                for (int i = 0; i < mailbox[idx].tag_count; i++) {
                    free(mailbox[idx].tags[i]);
                }
                free(mailbox[idx].tags);
                for (int i = idx; i < m_count - 1; i++) {
                    mailbox[i] = mailbox[i + 1];
                }
                m_count--;
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            for (int i = 0; i < m_count; i++) {
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
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < m_count; i++) {
                printf("%s %s %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
            }
        }
    }
    for (int i = 0; i < m_count; i++) {
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
