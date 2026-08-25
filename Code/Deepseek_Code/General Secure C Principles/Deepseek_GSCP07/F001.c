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

Para* find_para(const char *id) {
    Para *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void free_para(Para *p) {
    free(p->id);
    free(p->text);
    free(p);
}

void delete_para(const char *id) {
    Para *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free_para(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void list_all() {
    Para *p = head;
    while (p) {
        printf("%s\n", p->id);
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

void handle_new(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    char *text = strtok(NULL, "");
    if (!text) text = "";
    if (find_para(id)) return;
    Para *p = malloc(sizeof(Para));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (!head) head = p;
    else {
        Para *last = head;
        while (last->next) last = last->next;
        last->next = p;
    }
}

void handle_append(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    Para *p = find_para(id);
    if (!p) return;
    char *text = strtok(NULL, "");
    if (!text) return;
    size_t newlen = strlen(p->text) + strlen(text) + 1;
    char *newtext = malloc(newlen);
    sprintf(newtext, "%s%s", p->text, text);
    free(p->text);
    p->text = newtext;
}

void handle_replace(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    Para *p = find_para(id);
    if (!p) return;
    char *text = strtok(NULL, "");
    if (!text) return;
    free(p->text);
    p->text = strdup(text);
}

void handle_delete(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    delete_para(id);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "NEW") == 0) handle_new(cmd);
        else if (strcmp(cmd, "APPEND") == 0) handle_append(cmd);
        else if (strcmp(cmd, "REPLACE") == 0) handle_replace(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "SHOW") == 0) {
            char *id = strtok(NULL, " ");
            if (id) show_para(id);
        } else if (strcmp(cmd, "LIST") == 0) list_all();
    }
    while (head) {
        Para *tmp = head;
        head = head->next;
        free_para(tmp);
    }
    return 0;
}