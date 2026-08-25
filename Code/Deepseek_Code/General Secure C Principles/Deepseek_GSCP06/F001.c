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
    Para *p = malloc(sizeof(Para));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (!head) head = p;
    else {
        Para *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = p;
    }
}

void append_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (!p) return;
    char *newtext = malloc(strlen(p->text) + strlen(text) + 1);
    strcpy(newtext, p->text);
    strcat(newtext, text);
    free(p->text);
    p->text = newtext;
}

void replace_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (!p) return;
    free(p->text);
    p->text = strdup(text);
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
    for (char *c = p->text; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void list_paras(void) {
    Para *p = head;
    int first = 1;
    while (p) {
        if (!first) putchar(' ');
        printf("%s", p->id);
        first = 0;
        p = p->next;
    }
    putchar('\n');
}

void cleanup(void) {
    Para *p = head;
    while (p) {
        Para *tmp = p;
        p = p->next;
        free(tmp->id);
        free(tmp->text);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256], rest[2048];
        if (sscanf(line, "%31s %255s", cmd, id) < 2) continue;
        char *p = line + strlen(cmd) + 1 + strlen(id) + 1;
        if (*p == ' ') p++;
        if (strcmp(cmd, "NEW") == 0) {
            add_para(id, p);
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_para(id, p);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            replace_para(id, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_para(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_para(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paras();
        }
    }
    cleanup();
    return 0;
}