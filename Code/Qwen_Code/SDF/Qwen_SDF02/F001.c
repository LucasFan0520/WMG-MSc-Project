// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Para {
    char *id;
    char *text;
    struct Para *next;
} Para;

Para *head = NULL, *tail = NULL;

Para* find_para(const char *id) {
    for (Para *p = head; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "NEW") == 0) {
            char id[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", id, &n) != 1) continue;
            char *text = rest + n;
            while (*text == ' ') text++;
            if (find_para(id)) continue;
            Para *p = malloc(sizeof(Para));
            if (!p) continue;
            p->id = strdup(id);
            p->text = strdup(text);
            p->next = NULL;
            if (!head) head = tail = p;
            else { tail->next = p; tail = p; }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char id[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", id, &n) != 1) continue;
            char *text = rest + n;
            while (*text == ' ') text++;
            Para *p = find_para(id);
            if (!p) continue;
            size_t old_len = strlen(p->text);
            size_t add_len = strlen(text);
            char *new_text = malloc(old_len + 1 + add_len + 1);
            if (!new_text) continue;
            memcpy(new_text, p->text, old_len);
            new_text[old_len] = ' ';
            memcpy(new_text + old_len + 1, text, add_len + 1);
            free(p->text);
            p->text = new_text;
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char id[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", id, &n) != 1) continue;
            char *text = rest + n;
            while (*text == ' ') text++;
            Para *p = find_para(id);
            if (!p) continue;
            free(p->text);
            p->text = strdup(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) != 1) continue;
            Para *prev = NULL;
            for (Para *p = head; p; prev = p, p = p->next) {
                if (strcmp(p->id, id) == 0) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    if (tail == p) tail = prev;
                    free(p->id);
                    free(p->text);
                    free(p);
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) != 1) continue;
            Para *p = find_para(id);
            if (p) print_text(p->text);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Para *p = head; p; p = p->next) printf("%s\n", p->id);
        }
    }
    while (head) {
        Para *p = head;
        head = head->next;
        free(p->id);
        free(p->text);
        free(p);
    }
    return 0;
}