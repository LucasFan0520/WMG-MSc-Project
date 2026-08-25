// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Set { char *key; char *val; struct Set *next; } Set;
Set *committed = NULL, *staged = NULL; int in_tx = 0;

Set *find(Set *list, char *k) { for (Set *s = list; s; s = s->next) if (!strcmp(s->key, k)) return s; return NULL; }
void set_list(Set **list, char *k, char *v) {
    Set *s = find(*list, k);
    if (s) { free(s->val); s->val = strdup(v); }
    else { s = malloc(sizeof(Set)); s->key = strdup(k); s->val = strdup(v); s->next = *list; *list = s; }
}
void cmd_set(char *k, char *v) { if (in_tx) set_list(&staged, k, v); else set_list(&committed, k, v); }
void cmd_begin() { in_tx = 1; }
void cmd_tset(char *k, char *v) { if (in_tx) set_list(&staged, k, v); }
void cmd_commit() {
    if (!in_tx) return;
    for (Set *s = staged; s; s = s->next) set_list(&committed, s->key, s->val);
    Set *s = staged; while (s) { Set *ns = s->next; free(s->key); free(s->val); free(s); s = ns; }
    staged = NULL; in_tx = 0;
}
void cmd_abort() {
    if (!in_tx) return;
    Set *s = staged; while (s) { Set *ns = s->next; free(s->key); free(s->val); free(s); s = ns; }
    staged = NULL; in_tx = 0;
}
void cmd_get(char *k) { Set *s = find(committed, k); if (s) printf("%s\n", s->val); }
void cmd_dump() { for (Set *s = committed; s; s = s->next) printf("%s=%s\n", s->key, s->val); }
void free_all() {
    Set *s = committed; while (s) { Set *ns = s->next; free(s->key); free(s->val); free(s); s = ns; }
    s = staged; while (s) { Set *ns = s->next; free(s->key); free(s->val); free(s); s = ns; }
}
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "SET") && sscanf(line, "%*s %255s %4095[^\n]", a1, a2) >= 1) cmd_set(a1, a2);
        else if (!strcmp(cmd, "BEGIN")) cmd_begin();
        else if (!strcmp(cmd, "TSET") && sscanf(line, "%*s %255s %4095[^\n]", a1, a2) >= 1) cmd_tset(a1, a2);
        else if (!strcmp(cmd, "COMMIT")) cmd_commit();
        else if (!strcmp(cmd, "ABORT")) cmd_abort();
        else if (!strcmp(cmd, "GET") && sscanf(line, "%*s %255s", a1) == 1) cmd_get(a1);
        else if (!strcmp(cmd, "DUMP")) cmd_dump();
    }
    free_all(); return 0;
}