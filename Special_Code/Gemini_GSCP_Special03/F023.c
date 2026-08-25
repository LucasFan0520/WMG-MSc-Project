// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
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
    size_t tcount;
    size_t tcap;
} MailMessage;

int main(void) {
    MailMessage *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *body = p;
            size_t blen = strlen(body);
            while (blen > 0 && (body[blen-1] == '\n' || body[blen-1] == '\n')) {
                body[blen-1] = '\0';
                blen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                MailMessage *nitems = realloc(items, cap * sizeof(MailMessage));
                if (nitems) items = nitems;
            }
            items[count].sender = mystrdup(sender);
            items[count].subject = mystrdup(subject);
            items[count].body = mystrdup(body);
            items[count].tags = NULL;
            items[count].tcount = 0;
            items[count].tcap = 0;
            count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *tag = p;
            size_t tglen = strlen(tag);
            while (tglen > 0 && (tag[tglen-1] == '\n' || tag[tglen-1] == '\n')) {
                tag[tglen-1] = '\0';
                tglen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].subject, subject) == 0) {
                    MailMessage *m = &items[i];
                    if (m->tcount >= m->tcap) {
                        m->tcap = m->tcap == 0 ? 4 : m->tcap * 2;
                        char **nt = realloc(m->tags, m->tcap * sizeof(char*));
                        if (nt) m->tags = nt;
                    }
                    m->tags[m->tcount] = mystrdup(tag);
                    m->tcount++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *tag = p;
            size_t tglen = strlen(tag);
            while (tglen > 0 && (tag[tglen-1] == '\n' || tag[tglen-1] == '\n')) {
                tag[tglen-1] = '\0';
                tglen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].subject, subject) == 0) {
                    MailMessage *m = &items[i];
                    for (size_t j = 0; j < m->tcount; j++) {
                        if (strcmp(m->tags[j], tag) == 0) {
                            free(m->tags[j]);
                            for (size_t k = j; k < m->tcount - 1; k++) {
                                m->tags[k] = m->tags[k+1];
                            }
                            m->tcount--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = p;
            size_t slen = strlen(subject);
            while (slen > 0 && (subject[slen-1] == '\n' || subject[slen-1] == '\n')) {
                subject[slen-1] = '\0';
                slen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].subject, subject) == 0) {
                    free(items[i].sender);
                    free(items[i].subject);
                    free(items[i].body);
                    for (size_t j = 0; j < items[i].tcount; j++) {
                        free(items[i].tags[j]);
                    }
                    free(items[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = p;
            size_t tglen = strlen(tag);
            while (tglen > 0 && (tag[tglen-1] == '\n' || tag[tglen-1] == '\n')) {
                tag[tglen-1] = '\0';
                tglen--;
            }
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < items[i].tcount; j++) {
                    if (strcmp(items[i].tags[j], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", items[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("From: %s Subject: %s Body: %s Tags:", items[i].sender, items[i].subject, items[i].body);
                for (size_t j = 0; j < items[i].tcount; j++) {
                    printf(" %s", items[i].tags[j]);
                }
                putchar('\n');
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].sender);
        free(items[i].subject);
        free(items[i].body);
        for (size_t j = 0; j < items[i].tcount; j++) {
            free(items[i].tags[j]);
        }
        free(items[i].tags);
    }
    free(items);
    return 0;
}
