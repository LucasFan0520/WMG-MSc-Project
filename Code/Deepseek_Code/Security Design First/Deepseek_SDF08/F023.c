// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t tcount;
    size_t tcap;
} Message;

typedef struct {
    Message *items;
    size_t count;
    size_t capacity;
} Mailbox;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void msg_free(Message *m) {
    free(m->sender);
    free(m->subject);
    free(m->body);
    for (size_t i = 0; i < m->tcount; i++)
        free(m->tags[i]);
    free(m->tags);
    m->sender = NULL;
    m->subject = NULL;
    m->body = NULL;
    m->tags = NULL;
    m->tcount = 0;
    m->tcap = 0;
}

static void mb_init(Mailbox *mb) {
    mb->items = NULL;
    mb->count = 0;
    mb->capacity = 0;
}

static void mb_free(Mailbox *mb) {
    for (size_t i = 0; i < mb->count; i++)
        msg_free(&mb->items[i]);
    free(mb->items);
    mb->items = NULL;
    mb->count = 0;
    mb->capacity = 0;
}

static int mb_find_subject(Mailbox *mb, const char *subject) {
    for (size_t i = 0; i < mb->count; i++) {
        if (strcmp(mb->items[i].subject, subject) == 0)
            return (int)i;
    }
    return -1;
}

static void mb_mail(Mailbox *mb, const char *sender, const char *subject, const char *body) {
    if (mb_find_subject(mb, subject) >= 0) return;
    if (mb->count == mb->capacity) {
        size_t newcap = mb->capacity ? mb->capacity * 2 : 4;
        Message *tmp = realloc(mb->items, newcap * sizeof(Message));
        if (!tmp) return;
        mb->items = tmp;
        mb->capacity = newcap;
    }
    Message *m = &mb->items[mb->count];
    m->sender = safe_strdup(sender);
    m->subject = safe_strdup(subject);
    m->body = safe_strdup(body);
    m->tags = NULL;
    m->tcount = 0;
    m->tcap = 0;
    if (m->sender && m->subject && m->body)
        mb->count++;
    else {
        free(m->sender);
        free(m->subject);
        free(m->body);
    }
}

static void mb_tag(Mailbox *mb, const char *subject, const char *tag) {
    int idx = mb_find_subject(mb, subject);
    if (idx < 0) return;
    Message *m = &mb->items[idx];
    for (size_t i = 0; i < m->tcount; i++) {
        if (strcmp(m->tags[i], tag) == 0)
            return;
    }
    if (m->tcount == m->tcap) {
        size_t newcap = m->tcap ? m->tcap * 2 : 4;
        char **tmp = realloc(m->tags, newcap * sizeof(char *));
        if (!tmp) return;
        m->tags = tmp;
        m->tcap = newcap;
    }
    m->tags[m->tcount] = safe_strdup(tag);
    if (m->tags[m->tcount])
        m->tcount++;
}

static void mb_untag(Mailbox *mb, const char *subject, const char *tag) {
    int idx = mb_find_subject(mb, subject);
    if (idx < 0) return;
    Message *m = &mb->items[idx];
    size_t write = 0;
    for (size_t i = 0; i < m->tcount; i++) {
        if (strcmp(m->tags[i], tag) != 0) {
            if (write != i) m->tags[write] = m->tags[i];
            write++;
        } else {
            free(m->tags[i]);
        }
    }
    m->tcount = write;
}

static void mb_delete(Mailbox *mb, const char *subject) {
    int idx = mb_find_subject(mb, subject);
    if (idx < 0) return;
    msg_free(&mb->items[idx]);
    for (size_t i = idx; i < mb->count - 1; i++)
        mb->items[i] = mb->items[i+1];
    mb->count--;
}

static void mb_search(Mailbox *mb, const char *tag) {
    for (size_t i = 0; i < mb->count; i++) {
        Message *m = &mb->items[i];
        for (size_t j = 0; j < m->tcount; j++) {
            if (strcmp(m->tags[j], tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
        }
    }
}

static void mb_report(Mailbox *mb) {
    for (size_t i = 0; i < mb->count; i++) {
        Message *m = &mb->items[i];
        printf("%s %s ", m->sender, m->subject);
        const char *p = m->body;
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(void) {
    Mailbox mb;
    mb_init(&mb);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "MAIL") == 0) {
            mb_mail(&mb, a1, a2, a3);
        } else if (strcmp(cmd, "TAG") == 0) {
            mb_tag(&mb, a1, a2);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            mb_untag(&mb, a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            mb_delete(&mb, a1);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            mb_search(&mb, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            mb_report(&mb);
        }
    }
    mb_free(&mb);
    return 0;
}