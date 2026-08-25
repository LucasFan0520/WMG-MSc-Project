// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Para {
    char *id;
    char *text;
    struct Para *next;
} Para;

Para *head = NULL;

Para *find_para(const char *id) {
    Para *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_para(const char *id, const char *text) {
    if (find_para(id)) return;
    Para *newp = malloc(sizeof(Para));
    newp->id = malloc(strlen(id) + 1);
    strcpy(newp->id, id);
    newp->text = malloc(strlen(text) + 1);
    strcpy(newp->text, text);
    newp->next = NULL;
    if (!head) head = newp;
    else {
        Para *p = head;
        while (p->next) p = p->next;
        p->next = newp;
    }
}

void append_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (!p) return;
    size_t newlen = strlen(p->text) + strlen(text) + 1;
    char *newtext = malloc(newlen);
    strcpy(newtext, p->text);
    strcat(newtext, text);
    free(p->text);
    p->text = newtext;
}

void replace_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (!p) return;
    free(p->text);
    p->text = malloc(strlen(text) + 1);
    strcpy(p->text, text);
}

void delete_para(const char *id) {
    Para *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->id);
            free(p->text);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void show_para(const char *id) {
    Para *p = find_para(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *s = p->text; *s; s++) {
        if (*s == ' ') putchar('_');
        else putchar(*s);
    }
    putchar('\n');
}

void list_paras(void) {
    Para *p = head;
    while (p) {
        printf("%s\n", p->id);
        p = p->next;
    }
}

void free_all(void) {
    Para *p = head;
    while (p) {
        Para *tmp = p->next;
        free(p->id);
        free(p->text);
        free(p);
        p = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256];
        char text[4096];
        if (sscanf(line, "%31s %255s", cmd, id) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0) {
            if (strlen(rest) > strlen(id)) {
                add_para(id, rest + strlen(id) + 1);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (strlen(rest) > strlen(id)) {
                append_para(id, rest + strlen(id) + 1);
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            if (strlen(rest) > strlen(id)) {
                replace_para(id, rest + strlen(id) + 1);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_para(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_para(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paras();
        }
    }
    free_all();
    return 0;
}