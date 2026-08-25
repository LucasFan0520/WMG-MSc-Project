/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t t_count;
    size_t t_cap;
} Message;

int main(void) {
    Message *mailbox = NULL;
    size_t m_count = 0;
    size_t m_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
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
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            if (m_count >= m_cap) {
                m_cap = m_cap == 0 ? 4 : m_cap * 2;
                Message *nm = realloc(mailbox, m_cap * sizeof(Message));
                if (nm) mailbox = nm;
            }
            char *ns = mystrdup(sender);
            char *nsub = mystrdup(subject);
            char *nbody = mystrdup(body);
            if (ns && nsub && nbody) {
                mailbox[m_count].sender = ns;
                mailbox[m_count].subject = nsub;
                mailbox[m_count].body = nbody;
                mailbox[m_count].tags = NULL;
                mailbox[m_count].t_count = 0;
                mailbox[m_count].t_cap = 0;
                m_count++;
            } else {
                free(ns); free(nsub); free(nbody);
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mailbox[i].subject, subject) == 0) {
                    Message *m = &mailbox[i];
                    if (m->t_count >= m->t_cap) {
                        m->t_cap = m->t_cap == 0 ? 4 : m->t_cap * 2;
                        char **nt = realloc(m->tags, m->t_cap * sizeof(char *));
                        if (nt) m->tags = nt;
                    }
                    char *ntag = mystrdup(tag);
                    if (ntag) {
                        m->tags[m->t_count++] = ntag;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mailbox[i].subject, subject) == 0) {
                    Message *m = &mailbox[i];
                    int tidx = -1;
                    for (size_t j = 0; j < m->t_count; j++) {
                        if (strcmp(m->tags[j], tag) == 0) {
                            tidx = (int)j;
                            break;
                        }
                    }
                    if (tidx != -1) {
                        free(m->tags[tidx]);
                        for (size_t j = (size_t)tidx; j < m->t_count - 1; j++) {
                            m->tags[j] = m->tags[j + 1];
                        }
                        m->t_count--;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mailbox[i].subject, subject) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(mailbox[found].sender);
                free(mailbox[found].subject);
                free(mailbox[found].body);
                for (size_t j = 0; j < mailbox[found].t_count; j++) {
                    free(mailbox[found].tags[j]);
                }
                free(mailbox[found].tags);
                for (size_t i = (size_t)found; i < m_count - 1; i++) {
                    mailbox[i] = mailbox[i + 1];
                }
                m_count--;
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < m_count; i++) {
                int has_tag = 0;
                for (size_t j = 0; j < mailbox[i].t_count; j++) {
                    if (strcmp(mailbox[i].tags[j], tag) == 0) {
                        has_tag = 1;
                        break;
                    }
                }
                if (has_tag) {
                    printf("%s\n", mailbox[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < m_count; i++) {
                printf("%s %s %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < m_count; i++) {
        free(mailbox[i].sender);
        free(mailbox[i].subject);
        free(mailbox[i].body);
        for (size_t j = 0; j < mailbox[i].t_count; j++) {
            free(mailbox[i].tags[j]);
        }
        free(mailbox[i].tags);
    }
    free(mailbox);
    return 0;
}
