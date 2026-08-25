// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t tag_count;
    size_t tag_cap;
} Mail;

int main() {
    Mail *mails = NULL;
    size_t m_count = 0;
    size_t m_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            if (m_count >= m_cap) {
                m_cap = m_cap == 0 ? 4 : m_cap * 2;
                mails = realloc(mails, m_cap * sizeof(Mail));
            }
            mails[m_count].sender = mystrdup(arg1);
            mails[m_count].subject = mystrdup(subject);
            mails[m_count].body = mystrdup(body);
            mails[m_count].tags = NULL;
            mails[m_count].tag_count = 0;
            mails[m_count].tag_cap = 0;
            m_count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, arg1) == 0) {
                    if (mails[i].tag_count >= mails[i].tag_cap) {
                        mails[i].tag_cap = mails[i].tag_cap == 0 ? 4 : mails[i].tag_cap * 2;
                        mails[i].tags = realloc(mails[i].tags, mails[i].tag_cap * sizeof(char *));
                    }
                    mails[i].tags[mails[i].tag_count++] = mystrdup(tag);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, arg1) == 0) {
                    for (size_t j = 0; j < mails[i].tag_count; j++) {
                        if (strcmp(mails[i].tags[j], tag) == 0) {
                            free(mails[i].tags[j]);
                            for (size_t k = j; k < mails[i].tag_count - 1; k++) {
                                mails[i].tags[k] = mails[i].tags[k + 1];
                            }
                            mails[i].tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, arg1) == 0) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (size_t j = 0; j < mails[i].tag_count; j++) {
                        free(mails[i].tags[j]);
                    }
                    free(mails[i].tags);
                    for (size_t j = i; j < m_count - 1; j++) {
                        mails[j] = mails[j + 1];
                    }
                    m_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            for (size_t i = 0; i < m_count; i++) {
                for (size_t j = 0; j < mails[i].tag_count; j++) {
                    if (strcmp(mails[i].tags[j], arg1) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < m_count; i++) {
                printf("From: %s Subject: %s Body: %s Tags:", mails[i].sender, mails[i].subject, mails[i].body);
                for (size_t j = 0; j < mails[i].tag_count; j++) {
                    printf(" %s", mails[i].tags[j]);
                }
                printf("\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < m_count; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (size_t j = 0; j < mails[i].tag_count; j++) {
            free(mails[i].tags[j]);
        }
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}
