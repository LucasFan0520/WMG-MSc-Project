// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Enc { char *name; char *data; struct Enc *next; } Enc;
Enc *encs = NULL;

void cmd_store(char *n, char *d) { Enc *e = malloc(sizeof(Enc)); e->name = strdup(n); e->data = strdup(d); e->next = encs; encs = e; }
void cmd_decode(char *n) {
    Enc *e = encs; while (e) { if (!strcmp(e->name, n)) {
        char *p = e->data;
        while (*p) {
            if (!isdigit(*p)) return;
            long count = 0; while (isdigit(*p)) { count = count * 10 + (*p - '0'); p++; if (count > 1000000) return; }
            if (!*p) return;
            for (long i = 0; i < count; i++) { if (*p == ' ') putchar('_'); else putchar(*p); }
            p++;
        }
        putchar('\n'); return;
    } e = e->next; }
}
void cmd_delete(char *n) {
    Enc *curr = encs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else encs = curr->next; free(curr->name); free(curr->data); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_list() { for (Enc *e = encs; e; e = e->next) printf("%s\n", e->name); }
void free_all() { Enc *e = encs; while (e) { Enc *ne = e->next; free(e->name); free(e->data); free(e); e = ne; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "STORE") && sscanf(line, "%*s %255s %4095s", a1, a2) == 2) cmd_store(a1, a2);
        else if (!strcmp(cmd, "DECODE") && sscanf(line, "%*s %255s", a1) == 1) cmd_decode(a1);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "LIST")) cmd_list();
    }
    free_all(); return 0;
}