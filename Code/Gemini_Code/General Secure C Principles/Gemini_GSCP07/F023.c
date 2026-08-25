// F023.c
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
} MailMsg;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    MailMsg *mailbox = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            while (*p == ' ') p++;
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                MailMsg *nm = realloc(mailbox, capacity * sizeof(MailMsg));
                if (!nm) break;
                mailbox = nm;
            }
            mailbox[count].sender = strdup(sender);
            mailbox[count].subject = strdup(subject);
            mailbox[count].body = strdup(body);
            mailbox[count].tags = NULL;
            mailbox[count].tag_count = 0;
            mailbox[count].tag_capacity = 0;
            count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            while (*p == ' ') p++;
            char *subj = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subj) == 0) {
                    MailMsg *m = &mailbox[i];
                    if (m->tag_count >= m->tag_capacity) {
                        m->tag_capacity = m->tag_capacity == 0 ? 4 : m->tag_capacity * 2;
                        char **nt = realloc(m->tags, m->tag_capacity * sizeof(char *));
                        if (!nt) break;
                        m->tags = nt;
                    }
                    m->tags[m->tag_count++] = strdup(tag);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            while (*p == ' ') p++;
            char *subj = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subj) == 0) {
                    MailMsg *m = &mailbox[i];
                    for (size_t j = 0; j < m->tag_count; j++) {
                        if (strcmp(m->tags[j], tag) == 0) {
                            free(m->tags[j]);
                            for (size_t k = j; k < m->tag_count - 1; k++) {
                                m->tags[k] = m->tags[k + 1];
                            }
                            m->tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *subj = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subj) == 0) {
                    free(mailbox[i].sender);
                    free(mailbox[i].subject);
                    free(mailbox[i].body);
                    for (size_t j = 0; j < mailbox[i].tag_count; j++) {
                        free(mailbox[i].tags[j]);
                    }
                    free(mailbox[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        mailbox[j] = mailbox[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < mailbox[i].tag_count; j++) {
                    if (strcmp(mailbox[i].tags[j], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", mailbox[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("From: %s Subj: %s Body: %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(mailbox[i].sender);
        free(mailbox[i].subject);
        free(mailbox[i].body);
        for (size_t j = 0; j < mailbox[i].tag_count; j++) {
            free(mailbox[i].tags[j]);
        }
        free(mailbox[i].tags);
    }
    free(mailbox);
    return 0;
}
