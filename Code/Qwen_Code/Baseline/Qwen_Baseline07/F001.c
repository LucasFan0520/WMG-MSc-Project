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

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

void add_para(const char *id, const char *text) {
    Para *p = malloc(sizeof(Para));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (!head) head = tail = p;
    else { tail->next = p; tail = p; }
}

Para *find_para(const char *id) {
    for (Para *p = head; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

void print_underscore(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int main() {
    char cmd[20], id[256], text[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "NEW") == 0) {
            scanf("%s", id);
            read_rest(text, sizeof(text));
            add_para(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            scanf("%s", id);
            read_rest(text, sizeof(text));
            Para *p = find_para(id);
            if (p) {
                char *new_text = malloc(strlen(p->text) + strlen(text) + 1);
                strcpy(new_text, p->text);
                strcat(new_text, text);
                free(p->text);
                p->text = new_text;
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            scanf("%s", id);
            read_rest(text, sizeof(text));
            Para *p = find_para(id);
            if (p) {
                free(p->text);
                p->text = strdup(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", id);
            Para *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->id, id) == 0) {
                    if (prev) prev->next = curr->next;
                    else head = curr->next;
                    if (!curr->next) tail = prev;
                    free(curr->id); free(curr->text); free(curr);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", id);
            Para *p = find_para(id);
            if (p) print_underscore(p->text);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Para *p = head; p; p = p->next) printf("%s\n", p->id);
        }
    }
    return 0;
}