// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct B {
    char *line;
    struct B *next;
} B;

typedef struct R {
    char *title;
    B *body;
    struct R *next;
} R;

R *head = NULL;

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    R *curr = NULL;
    B *btail = NULL;
    while (fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "--")) {
            if (curr) {
                curr->next = head;
                head = curr;
                curr = NULL;
                btail = NULL;
            }
        } else {
            if (!curr) {
                curr = malloc(sizeof(R));
                curr->title = strdup(line);
                curr->body = NULL;
            } else {
                B *nb = malloc(sizeof(B));
                nb->line = strdup(line);
                nb->next = NULL;
                if (!curr->body) {
                    curr->body = btail = nb;
                } else {
                    btail->next = nb;
                    btail = nb;
                }
            }
        }
    }
    if (curr) {
        curr->next = head;
        head = curr;
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char title[1000];
    char line[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "FIND")) {
            fgets(title, 1000, stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') {
                memmove(title, title + 1, strlen(title));
            }
            R *p = head;
            while (p && strcmp(p->title, title)) {
                p = p->next;
            }
            if (p) {
                printf("%s\n", p->title);
                for (B *b = p->body; b; b = b->next) {
                    printf("%s\n", b->line);
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            fgets(title, 1000, stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') {
                memmove(title, title + 1, strlen(title));
            }
            R *p = head;
            R *prev = NULL;
            while (p) {
                if (!strcmp(p->title, title)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    B *b = p->body;
                    while (b) {
                        B *tmp = b;
                        b = b->next;
                        free(tmp->line);
                        free(tmp);
                    }
                    free(p->title);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "ADD")) {
            fgets(title, 1000, stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') {
                memmove(title, title + 1, strlen(title));
            }
            R *nr = malloc(sizeof(R));
            nr->title = strdup(title);
            nr->body = NULL;
            nr->next = head;
            head = nr;
            B *btail = NULL;
            while (fgets(line, 1000, stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (!strcmp(line, "--")) {
                    break;
                }
                B *nb = malloc(sizeof(B));
                nb->line = strdup(line);
                nb->next = NULL;
                if (!nr->body) {
                    nr->body = btail = nb;
                } else {
                    btail->next = nb;
                    btail = nb;
                }
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (R *p = head; p; p = p->next) {
                printf("%s\n", p->title);
                for (B *b = p->body; b; b = b->next) {
                    printf("%s\n", b->line);
                }
                printf("--\n");
            }
        }
    }
    return 0;
}