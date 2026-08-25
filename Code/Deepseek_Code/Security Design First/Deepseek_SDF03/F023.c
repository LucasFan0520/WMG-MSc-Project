// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_SENDER 64
#define MAX_SUBJECT 64
#define MAX_BODY 4096
#define MAX_TAG 64

typedef struct Message {
    char sender[MAX_SENDER];
    char subject[MAX_SUBJECT];
    char body[MAX_BODY];
    char tags[MAX_LINE]; // space-separated tags
    struct Message *next;
} Message;

Message *head = NULL;

Message *find_by_subject(const char *subject) {
    Message *m = head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void free_all() {
    Message *m = head;
    while (m) {
        Message *tmp = m;
        m = m->next;
        free(tmp);
    }
    head = NULL;
}

void handle_mail(char *line) {
    char sender[MAX_SENDER], subject[MAX_SUBJECT], body[MAX_BODY];
    int n = sscanf(line, "MAIL %63s %63s %4095[^\n]", sender, subject, body);
    if (n < 3) return;
    if (find_by_subject(subject)) return;
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    strcpy(m->sender, sender);
    strcpy(m->subject, subject);
    strcpy(m->body, body);
    m->tags[0] = '\0';
    m->next = head;
    head = m;
}

void handle_tag(char *line) {
    char subject[MAX_SUBJECT], tag[MAX_TAG];
    if (sscanf(line, "TAG %63s %63s", subject, tag) != 2) return;
    Message *m = find_by_subject(subject);
    if (!m) return;
    if (strstr(m->tags, tag)) return;
    if (m->tags[0]) strcat(m->tags, " ");
    strcat(m->tags, tag);
}

void handle_untag(char *line) {
    char subject[MAX_SUBJECT], tag[MAX_TAG];
    if (sscanf(line, "UNTAG %63s %63s", subject, tag) != 2) return;
    Message *m = find_by_subject(subject);
    if (!m) return;
    char *p = strstr(m->tags, tag);
    if (!p) return;
    int len = strlen(tag);
    if (p > m->tags && *(p-1) == ' ') p--;
    char *end = p + len;
    if (*end == ' ') end++;
    memmove(p, end, strlen(end)+1);
}

void handle_delete(char *line) {
    char subject[MAX_SUBJECT];
    if (sscanf(line, "DELETE %63s", subject) != 1) return;
    Message *m = head, *prev = NULL;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void handle_search(char *line) {
    char tag[MAX_TAG];
    if (sscanf(line, "SEARCH %63s", tag) != 1) return;
    Message *m = head;
    int first = 1;
    while (m) {
        if (strstr(m->tags, tag)) {
            if (!first) putchar(' ');
            printf("%s", m->subject);
            first = 0;
        }
        m = m->next;
    }
    putchar('\n');
}

void handle_report() {
    Message *m = head;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        if (m->tags[0]) printf(" %s", m->tags);
        putchar('\n');
        m = m->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "MAIL ", 5) == 0) handle_mail(line);
        else if (strncmp(line, "TAG ", 4) == 0) handle_tag(line);
        else if (strncmp(line, "UNTAG ", 6) == 0) handle_untag(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "SEARCH ", 7) == 0) handle_search(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}