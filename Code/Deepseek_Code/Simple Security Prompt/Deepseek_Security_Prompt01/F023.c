// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *subject; char *sender; char *body; char **tags; int tc, tcap; } Message;
Message *msgs = NULL;
int mc = 0, mcap = 0;

int find_msg(const char *subject) {
    for (int i = 0; i < mc; i++)
        if (strcmp(msgs[i].subject, subject) == 0) return i;
    return -1;
}

int find_tag(Message *m, const char *tag) {
    for (int i = 0; i < m->tc; i++)
        if (strcmp(m->tags[i], tag) == 0) return i;
    return -1;
}

void add_message(const char *sender, const char *subject, const char *body) {
    if (find_msg(subject) >= 0) return;
    if (mc == mcap) { mcap = mcap ? mcap * 2 : 4; msgs = realloc(msgs, sizeof(Message) * mcap); }
    msgs[mc].sender = strdup(sender);
    msgs[mc].subject = strdup(subject);
    msgs[mc].body = strdup(body);
    msgs[mc].tags = NULL;
    msgs[mc].tc = 0;
    msgs[mc].tcap = 0;
    mc++;
}

void tag_message(const char *subject, const char *tag) {
    int idx = find_msg(subject);
    if (idx < 0) return;
    Message *m = &msgs[idx];
    if (find_tag(m, tag) >= 0) return;
    if (m->tc == m->tcap) { m->tcap = m->tcap ? m->tcap * 2 : 4; m->tags = realloc(m->tags, sizeof(char *) * m->tcap); }
    m->tags[m->tc] = strdup(tag);
    m->tc++;
}

void untag_message(const char *subject, const char *tag) {
    int idx = find_msg(subject);
    if (idx < 0) return;
    Message *m = &msgs[idx];
    int ti = find_tag(m, tag);
    if (ti < 0) return;
    free(m->tags[ti]);
    for (int i = ti; i < m->tc - 1; i++) m->tags[i] = m->tags[i + 1];
    m->tc--;
}

void delete_message(const char *subject) {
    int idx = find_msg(subject);
    if (idx < 0) return;
    free(msgs[idx].sender);
    free(msgs[idx].subject);
    free(msgs[idx].body);
    for (int i = 0; i < msgs[idx].tc; i++) free(msgs[idx].tags[i]);
    free(msgs[idx].tags);
    for (int i = idx; i < mc - 1; i++) msgs[i] = msgs[i + 1];
    mc--;
}

void search_tag(const char *tag) {
    for (int i = 0; i < mc; i++) {
        for (int j = 0; j < msgs[i].tc; j++) {
            if (strcmp(msgs[i].tags[j], tag) == 0) {
                printf("%s\n", msgs[i].subject);
                break;
            }
        }
    }
}

void report_messages(void) {
    for (int i = 0; i < mc; i++) {
        printf("%s %s %s", msgs[i].sender, msgs[i].subject, msgs[i].body);
        for (int j = 0; j < msgs[i].tc; j++) printf(" %s", msgs[i].tags[j]);
        putchar('\n');
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", arg1, arg2, arg3) == 3)
                add_message(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                tag_message(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                untag_message(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_message(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) report_messages();
    }
    for (int i = 0; i < mc; i++) {
        free(msgs[i].sender);
        free(msgs[i].subject);
        free(msgs[i].body);
        for (int j = 0; j < msgs[i].tc; j++) free(msgs[i].tags[j]);
        free(msgs[i].tags);
    }
    free(msgs);
    return 0;
}