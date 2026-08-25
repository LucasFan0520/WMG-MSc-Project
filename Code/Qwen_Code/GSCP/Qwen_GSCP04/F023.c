// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tag { char *name; struct Tag *next; } Tag;
typedef struct Mail { char *sender; char *subject; char *body; Tag *tags; struct Mail *next; } Mail;
Mail *mails = NULL;

Mail *find_mail_subj(char *s) { for (Mail *m = mails; m; m = m->next) if (!strcmp(m->subject, s)) return m; return NULL; }

void cmd_mail(char *s, char *sub, char *b) {
    Mail *m = malloc(sizeof(Mail)); m->sender = strdup(s); m->subject = strdup(sub); m->body = strdup(b); m->tags = NULL; m->next = mails; mails = m;
}
void cmd_tag(char *sub, char *t) {
    Mail *m = find_mail_subj(sub); if (!m) return;
    Tag *tag = malloc(sizeof(Tag)); tag->name = strdup(t); tag->next = m->tags; m->tags = tag;
}
void cmd_untag(char *sub, char *t) {
    Mail *m = find_mail_subj(sub); if (!m) return;
    Tag *curr = m->tags, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, t)) { if (prev) prev->next = curr->next; else m->tags = curr->next; free(curr->name); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_delete(char *sub) {
    Mail *curr = mails, *prev = NULL;
    while (curr) { if (!strcmp(curr->subject, sub)) { if (prev) prev->next = curr->next; else mails = curr->next; Tag *t = curr->tags; while (t) { Tag *nt = t->next; free(t->name); free(t); t = nt; } free(curr->sender); free(curr->subject); free(curr->body); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_search(char *t) {
    for (Mail *m = mails; m; m = m->next) {
        for (Tag *tag = m->tags; tag; tag = tag->next) {
            if (!strcmp(tag->name, t)) { printf("%s\n", m->subject); break; }
        }
    }
}
void cmd_report() {
    for (Mail *m = mails; m; m = m->next) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        for (Tag *t = m->tags; t; t = t->next) printf(" %s", t->name);
        printf("\n");
    }
}
void free_all() {
    Mail *m = mails; while (m) { Mail *nm = m->next; Tag *t = m->tags; while (t) { Tag *nt = t->next; free(t->name); free(t); t = nt; } free(m->sender); free(m->subject); free(m->body); free(m); m = nm; }
}
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256], a3[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "MAIL") && sscanf(line, "%*s %255s %255s %4095[^\n]", a1, a2, a3) >= 2) cmd_mail(a1, a2, a3);
        else if (!strcmp(cmd, "TAG") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_tag(a1, a2);
        else if (!strcmp(cmd, "UNTAG") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_untag(a1, a2);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "SEARCH") && sscanf(line, "%*s %255s", a1) == 1) cmd_search(a1);
        else if (!strcmp(cmd, "REPORT")) cmd_report();
    }
    free_all(); return 0;
}