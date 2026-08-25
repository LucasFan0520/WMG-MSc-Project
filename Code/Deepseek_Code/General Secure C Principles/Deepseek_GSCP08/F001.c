// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Para { char *id; char *text; struct Para *next; } Para;
Para *head = NULL, *tail = NULL;

Para *find_para(const char *id) {
    for (Para *p = head; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

void new_para(const char *id, const char *text) {
    if (find_para(id)) return;
    Para *p = malloc(sizeof(Para));
    if (!p) return;
    p->id = strdup(id); p->text = strdup(text); p->next = NULL;
    if (tail) tail->next = p; else head = p;
    tail = p;
}

void append_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (!p) return;
    size_t newlen = strlen(p->text) + strlen(text) + 1;
    char *tmp = realloc(p->text, newlen);
    if (!tmp) return;
    p->text = tmp; strcat(p->text, text);
}

void replace_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (!p) return;
    free(p->text); p->text = strdup(text);
}

void delete_para(const char *id) {
    Para *prev = NULL;
    for (Para *p = head; p; prev = p, p = p->next) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next; else head = p->next;
            if (p == tail) tail = prev;
            free(p->id); free(p->text); free(p);
            return;
        }
    }
}

void show_para(const char *id) {
    Para *p = find_para(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *c = p->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void list_paras(void) {
    for (Para *p = head; p; p = p->next) printf("%s\n", p->id);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16]; int n;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0) {
            char id[256], text[4096];
            if (sscanf(rest, "%255s", id) == 1) {
                char *t = rest + strlen(id);
                while (*t == ' ') t++;
                strcpy(text, t);
                new_para(id, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char id[256], text[4096];
            if (sscanf(rest, "%255s", id) == 1) {
                char *t = rest + strlen(id);
                while (*t == ' ') t++;
                strcpy(text, t);
                append_para(id, text);
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char id[256], text[4096];
            if (sscanf(rest, "%255s", id) == 1) {
                char *t = rest + strlen(id);
                while (*t == ' ') t++;
                strcpy(text, t);
                replace_para(id, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) == 1) delete_para(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) == 1) show_para(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paras();
        }
    }
    // cleanup
    while (head) { Para *tmp = head; head = head->next; free(tmp->id); free(tmp->text); free(tmp); }
    return 0;
}