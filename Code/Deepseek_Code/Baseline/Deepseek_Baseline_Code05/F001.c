/* F001.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct para {
    char *id;
    char *text;
    struct para *next;
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

void new_para(char *id, char *text) {
    Para *exist = find_para(id);
    if (exist) {
        free(exist->text);
        exist->text = strdup(text);
        return;
    }
    Para *p = malloc(sizeof(Para));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = head;
    head = p;
}

void append_para(char *id, char *text) {
    Para *p = find_para(id);
    if (!p) return;
    char *newtext = malloc(strlen(p->text) + strlen(text) + 1);
    strcpy(newtext, p->text);
    strcat(newtext, text);
    free(p->text);
    p->text = newtext;
}

void replace_para(char *id, char *text) {
    Para *p = find_para(id);
    if (!p) return;
    free(p->text);
    p->text = strdup(text);
}

void delete_para(char *id) {
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

void show_para(char *id) {
    Para *p = find_para(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *c = p->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void list_all() {
    Para *p = head;
    while (p) {
        printf("%s\n", p->id);
        p = p->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], id[256], text[1024];
        if (sscanf(line, "%15s %255s", cmd, id) < 2) continue;
        char *rest = line + strlen(cmd) + 1 + strlen(id) + 1;
        if (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0) new_para(id, rest);
        else if (strcmp(cmd, "APPEND") == 0) append_para(id, rest);
        else if (strcmp(cmd, "REPLACE") == 0) replace_para(id, rest);
        else if (strcmp(cmd, "DELETE") == 0) delete_para(id);
        else if (strcmp(cmd, "SHOW") == 0) show_para(id);
        else if (strcmp(cmd, "LIST") == 0) list_all();
    }
    return 0;
}