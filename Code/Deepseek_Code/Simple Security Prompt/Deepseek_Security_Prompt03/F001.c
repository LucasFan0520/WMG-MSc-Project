/*
 * F001.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct para {
    char *id;
    char *text;
    struct para *next;
} Para;

Para *head = NULL, *tail = NULL;

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
    if (tail) tail->next = p;
    else head = p;
    tail = p;
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
    Para *prev = NULL, *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (p == tail) tail = prev;
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
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = p->text; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256], text[4096];
        if (sscanf(line, "%31s %255s", cmd, id) < 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        char *idstart = rest;
        while (*rest && *rest != ' ') rest++;
        if (*rest) {
            *rest = 0;
            rest++;
            while (*rest == ' ') rest++;
            strcpy(text, rest);
        } else {
            text[0] = 0;
        }
        if (strcmp(cmd, "NEW") == 0) {
            add_para(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_para(id, text);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            replace_para(id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_para(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_para(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paras();
        }
    }
    return 0;
}