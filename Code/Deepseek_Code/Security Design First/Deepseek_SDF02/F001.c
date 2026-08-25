/* F001.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct para {
    char *id;
    char *text;
    struct para *next;
} para_t;

para_t *head = NULL;
para_t *tail = NULL;

para_t *find_para(const char *id) {
    para_t *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_para(const char *id, const char *text) {
    if (find_para(id)) return;
    para_t *p = malloc(sizeof(para_t));
    if (!p) return;
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (!p->id || !p->text) {
        free(p->id); free(p->text); free(p);
        return;
    }
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void append_para(const char *id, const char *text) {
    para_t *p = find_para(id);
    if (!p) return;
    size_t newlen = strlen(p->text) + strlen(text) + 1;
    char *newtext = realloc(p->text, newlen);
    if (!newtext) return;
    p->text = newtext;
    strcat(p->text, text);
}

void replace_para(const char *id, const char *text) {
    para_t *p = find_para(id);
    if (!p) return;
    char *newtext = strdup(text);
    if (!newtext) return;
    free(p->text);
    p->text = newtext;
}

void delete_para(const char *id) {
    para_t *prev = NULL, *p = head;
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
    para_t *p = find_para(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = p->text; *s; s++) {
        if (*s == ' ') putchar('_');
        else putchar(*s);
    }
    putchar('\n');
}

void list_paras(void) {
    para_t *p = head;
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
    para_t *p = head;
    while (p) {
        para_t *next = p->next;
        free(p->id);
        free(p->text);
        free(p);
        p = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], id[256];
        if (sscanf(line, "%9s %255s", cmd, id) != 2) continue;
        const char *text = line + strlen(cmd) + 1 + strlen(id) + 1;
        if (*text == ' ') text++; // skip extra space
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
    free(line);
    free_all();
    return 0;
}