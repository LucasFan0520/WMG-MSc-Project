// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Label {
    char *text;
    struct Label *next;
} Label;

typedef struct Item {
    char *name, *note;
    Label *labels;
    struct Item *next;
} Item;

Item *head = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main() {
    char cmd[20], name[256], text[256];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ITEM") == 0) {
            scanf("%s", name); read_rest(text);
            Item *it = malloc(sizeof(Item));
            it->name = strdup(name); it->note = strdup(text); it->labels = NULL; it->next = head; head = it;
        } else if (strcmp(cmd, "LABEL") == 0) {
            scanf("%s %s", name, text);
            for (Item *it = head; it; it = it->next)
                if (strcmp(it->name, name) == 0) {
                    Label *l = malloc(sizeof(Label));
                    l->text = strdup(text); l->next = it->labels; it->labels = l; break;
                }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            scanf("%s %s", name, text);
            for (Item *it = head; it; it = it->next)
                if (strcmp(it->name, name) == 0) {
                    Label *prev = NULL, *curr = it->labels;
                    while (curr) {
                        if (strcmp(curr->text, text) == 0) {
                            if (prev) prev->next = curr->next; else it->labels = curr->next;
                            free(curr->text); free(curr); break;
                        }
                        prev = curr; curr = curr->next;
                    }
                    break;
                }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            Item *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    while (curr->labels) { Label *nl = curr->labels->next; free(curr->labels->text); free(curr->labels); curr->labels = nl; }
                    free(curr->name); free(curr->note); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            for (Item *it = head; it; it = it->next)
                if (strcmp(it->name, name) == 0) {
                    printf("%s %s", it->name, it->note);
                    for (Label *l = it->labels; l; l = l->next) printf(" %s", l->text);
                    printf("\n"); break;
                }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item *it = head; it; it = it->next) {
                printf("%s %s", it->name, it->note);
                for (Label *l = it->labels; l; l = l->next) printf(" %s", l->text);
                printf("\n");
            }
        }
    }
    return 0;
}