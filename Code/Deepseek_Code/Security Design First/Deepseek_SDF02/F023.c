/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tag {
    char *name;
    struct tag *next;
} tag_t;

typedef struct msg {
    char *sender;
    char *subject;
    char *body;
    tag_t *tags;
    struct msg *next;
} msg_t;

msg_t *msgs = NULL;
msg_t *tail = NULL;

msg_t *find_msg_by_subject(const char *subject) {
    msg_t *m = msgs;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void mail_cmd(const char *sender, const char *subject, const char *body) {
    msg_t *m = malloc(sizeof(msg_t));
    if (!m) return;
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = NULL;
    if (!m->sender || !m->subject || !m->body) {
        free(m->sender); free(m->subject); free(m->body); free(m);
        return;
    }
    if (tail) tail->next = m;
    else msgs = m;
    tail = m;
}

void tag_cmd(const char *subject, const char *tagname) {
    msg_t *m = find_msg_by_subject(subject);
    if (!m) return;
    tag_t *t = m->tags;
    while (t) {
        if (strcmp(t->name, tagname) == 0) return;
        t = t->next;
    }
    tag_t *new = malloc(sizeof(tag_t));
    if (!new) return;
    new->name = strdup(tagname);
    new->next = m->tags;
    m->tags = new;
    if (!new->name) { free(new); }
}

void untag_cmd(const char *subject, const char *tagname) {
    msg_t *m = find_msg_by_subject(subject);
    if (!m) return;
    tag_t *prev = NULL, *t = m->tags;
    while (t) {
        if (strcmp(t->name, tagname) == 0) {
            if (prev) prev->next = t->next;
            else m->tags = t->next;
            free(t->name);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void delete_cmd(const char *subject) {
    msg_t *prev = NULL, *m = msgs;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else msgs = m->next;
            if (m == tail) tail = prev;
            free(m->sender);
            free(m->subject);
            free(m->body);
            tag_t *t = m->tags;
            while (t) {
                tag_t *nxt = t->next;
                free(t->name);
                free(t);
                t = nxt;
            }
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void search_cmd(const char *tagname) {
    msg_t *m = msgs;
    while (m) {
        tag_t *t = m->tags;
        while (t) {
            if (strcmp(t->name, tagname) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void report_cmd(void) {
    msg_t *m = msgs;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        tag_t *t = m->tags;
        while (t) {
            printf(" %s", t->name);
            t = t->next;
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all(void) {
    msg_t *m = msgs;
    while (m) {
        msg_t *nxt = m->next;
        free(m->sender);
        free(m->subject);
        free(m->body);
        tag_t *t = m->tags;
        while (t) {
            tag_t *n = t->next;
            free(t->name);
            free(t);
            t = n;
        }
        free(m);
        m = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        if (sscanf(line, "%9s %255s %255s", cmd, a, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "MAIL") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2) {
                    const char *body = line + strlen(cmd) + 1 + strlen(a) + 1 + strlen(b) + 1;
                    mail_cmd(a, b, body);
                }
            } else if (strcmp(cmd, "TAG") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    tag_cmd(a, b);
            } else if (strcmp(cmd, "UNTAG") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    untag_cmd(a, b);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_cmd(a);
            } else if (strcmp(cmd, "SEARCH") == 0) {
                search_cmd(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_cmd();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}