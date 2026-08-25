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

Para *find_para(char *id) {
    for (Para *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) return p;
    }
    return NULL;
}

void add_para(char *id, char *text) {
    Para *p = find_para(id);
    if (!p) {
        p = malloc(sizeof(Para));
        p->id = strdup(id);
        p->next = NULL;
        if (!head) {
            head = p;
        } else {
            Para *last = head;
            while (last->next) last = last->next;
            last->next = p;
        }
    } else {
        free(p->text);
    }
    p->text = strdup(text);
}

void append_para(char *id, char *text) {
    Para *p = find_para(id);
    if (!p) {
        add_para(id, text);
        return;
    }
    char *new_text = malloc(strlen(p->text) + strlen(text) + 2);
    sprintf(new_text, "%s %s", p->text, text);
    free(p->text);
    p->text = new_text;
}

void replace_para(char *id, char *text) {
    Para *p = find_para(id);
    if (p) {
        free(p->text);
        p->text = strdup(text);
    }
}

void delete_para(char *id) {
    Para *prev = NULL;
    Para *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->id);
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_para(char *id) {
    Para *p = find_para(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = p->text; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void list_paras() {
    for (Para *p = head; p; p = p->next) {
        printf("%s\n", p->id);
    }
}

int main() {
    char cmd[100], id[100], text[2000];
    while (scanf("%99s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            scanf("%99s", id);
            fgets(text, sizeof(text), stdin);
            while (text[0] == ' ') memmove(text, text + 1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            if (strcmp(cmd, "NEW") == 0) add_para(id, text);
            else if (strcmp(cmd, "APPEND") == 0) append_para(id, text);
            else replace_para(id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", id);
            delete_para(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%99s", id);
            show_para(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paras();
        }
    }
    return 0;
}