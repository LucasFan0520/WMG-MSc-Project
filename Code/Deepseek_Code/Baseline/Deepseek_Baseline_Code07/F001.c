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
Para *tail = NULL;

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
    p->id = malloc(strlen(id)+1);
    strcpy(p->id, id);
    p->text = malloc(strlen(text)+1);
    strcpy(p->text, text);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
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
    p->text = malloc(strlen(text)+1);
    strcpy(p->text, text);
}

void delete_para(const char *id) {
    Para *prev = NULL;
    Para *p = head;
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
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *c = p->text; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void list_all(void) {
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

void free_all(void) {
    Para *p = head;
    while (p) {
        Para *next = p->next;
        free(p->id);
        free(p->text);
        free(p);
        p = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "NEW") == 0) {
            char *id = strtok(rest, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            add_para(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *id = strtok(rest, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            append_para(id, text);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *id = strtok(rest, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            replace_para(id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = strtok(rest, " ");
            if (id) delete_para(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = strtok(rest, " ");
            if (id) show_para(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }
    free_all();
    return 0;
}