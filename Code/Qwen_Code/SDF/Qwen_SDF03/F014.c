// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name, *note;
    char **labels;
    size_t lcount, lcap;
    struct Item *next;
} Item;

Item *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Item *find_item(const char *name) {
    for (Item *i = head; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = get_token(&ptr);
            if (name && !find_item(name)) {
                Item *i = malloc(sizeof(Item));
                if (i) { i->name = strdup(name); i->note = strdup(ptr); i->labels = NULL; i->lcount = i->lcap = 0; i->next = head; head = i; }
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = get_token(&ptr);
            char *label = get_token(&ptr);
            if (name && label) {
                Item *i = find_item(name);
                if (i) {
                    if (i->lcount == i->lcap) {
                        size_t new_cap = i->lcap ? i->lcap * 2 : 4;
                        char **tmp = realloc(i->labels, new_cap * sizeof(char *));
                        if (tmp) { i->labels = tmp; i->lcap = new_cap; }
                    }
                    i->labels[i->lcount++] = strdup(label);
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = get_token(&ptr);
            char *label = get_token(&ptr);
            if (name && label) {
                Item *i = find_item(name);
                if (i) {
                    for (size_t j = 0; j < i->lcount; j++) {
                        if (strcmp(i->labels[j], label) == 0) {
                            free(i->labels[j]);
                            i->labels[j] = i->labels[--i->lcount];
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Item *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        for (size_t j = 0; j < curr->lcount; j++) free(curr->labels[j]);
                        free(curr->labels); free(curr->name); free(curr->note); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Item *i = find_item(name);
                if (i) {
                    printf("%s %s", i->name, i->note);
                    for (size_t j = 0; j < i->lcount; j++) printf(" %s", i->labels[j]);
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item *i = head; i; i = i->next) {
                printf("%s %s", i->name, i->note);
                for (size_t j = 0; j < i->lcount; j++) printf(" %s", i->labels[j]);
                printf("\n");
            }
        }
    }
    while (head) {
        Item *next = head->next;
        for (size_t j = 0; j < head->lcount; j++) free(head->labels[j]);
        free(head->labels); free(head->name); free(head->note); free(head);
        head = next;
    }
    return 0;
}