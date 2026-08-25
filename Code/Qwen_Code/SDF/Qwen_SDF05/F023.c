// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t tcnt, tcap;
} Mail;

static Mail *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Mail *t = realloc(arr, nc * sizeof(Mail));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int tag_grow(Mail *m) {
    if (m->tcnt < m->tcap) return 0;
    size_t nc = m->tcap ? m->tcap * 2 : 4;
    if (nc < m->tcap) return -1;
    char **t = realloc(m->tags, nc * sizeof(char *));
    if (!t) return -1;
    m->tags = t; m->tcap = nc;
    return 0;
}

static int find_mail_subject(const char *subject) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].subject, subject) == 0) return (int)i;
    return -1;
}

static void free_mail(Mail *m) {
    free(m->sender); free(m->subject); free(m->body);
    for (size_t i = 0; i < m->tcnt; i++) free(m->tags[i]);
    free(m->tags);
    m->sender = NULL; m->subject = NULL; m->body = NULL; m->tags = NULL;
    m->tcnt = 0; m->tcap = 0;
}

static void cmd_mail(char *sender, char *subject, char *body) {
    if (grow() < 0) return;
    arr[cnt].sender = safe_strdup(sender);
    arr[cnt].subject = safe_strdup(subject);
    arr[cnt].body = safe_strdup(body);
    arr[cnt].tags = NULL; arr[cnt].tcnt = 0; arr[cnt].tcap = 0;
    if (!arr[cnt].sender || !arr[cnt].subject || !arr[cnt].body) {
        free_mail(&arr[cnt]);
        return;
    }
    cnt++;
}

static void cmd_tag(char *subject, char *tag) {
    int idx = find_mail_subject(subject);
    if (idx < 0) return;
    Mail *m = &arr[idx];
    for (size_t i = 0; i < m->tcnt; i++)
        if (strcmp(m->tags[i], tag) == 0) return;
    if (tag_grow(m) < 0) return;
    m->tags[m->tcnt] = safe_strdup(tag);
    if (m->tags[m->tcnt]) m->tcnt++;
}

static void cmd_untag(char *subject, char *tag) {
    int idx = find_mail_subject(subject);
    if (idx < 0) return;
    Mail *m = &arr[idx];
    for (size_t i = 0; i < m->tcnt; i++) {
        if (strcmp(m->tags[i], tag) == 0) {
            free(m->tags[i]);
            for (size_t j = i; j + 1 < m->tcnt; j++) m->tags[j] = m->tags[j + 1];
            m->tcnt--;
            return;
        }
    }
}

static void cmd_delete(char *subject) {
    int idx = find_mail_subject(subject);
    if (idx < 0) return;
    free_mail(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_search(char *tag) {
    for (size_t i = 0; i < cnt; i++) {
        for (size_t j = 0; j < arr[i].tcnt; j++) {
            if (strcmp(arr[i].tags[j], tag) == 0) {
                printf("%s\n", arr[i].subject);
                break;
            }
        }
    }
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++) {
        printf("%s %s %s", arr[i].sender, arr[i].subject, arr[i].body);
        for (size_t j = 0; j < arr[i].tcnt; j++) printf(" %s", arr[i].tags[j]);
        putchar('\n');
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_mail(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256] = {0}, subject[256] = {0}; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%255s%n %255s%n", sender, &n2, subject, &n3) < 2) continue;
            char *body = rest + n3;
            while (*body == ' ') body++;
            cmd_mail(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0 || strcmp(cmd, "UNTAG") == 0) {
            char subject[256] = {0}, tag[256] = {0};
            if (sscanf(rest, "%255s %255s", subject, tag) < 2) continue;
            if (strcmp(cmd, "TAG") == 0) cmd_tag(subject, tag);
            else cmd_untag(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[256] = {0};
            if (sscanf(rest, "%255s", subject) < 1) continue;
            cmd_delete(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256] = {0};
            if (sscanf(rest, "%255s", tag) < 1) continue;
            cmd_search(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}