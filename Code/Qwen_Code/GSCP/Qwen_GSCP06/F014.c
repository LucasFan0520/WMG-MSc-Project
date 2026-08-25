// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Label {
    char* name;
    struct Label* next;
} Label;

typedef struct Item {
    char* name;
    char* note;
    Label* labels;
    struct Item* next;
} Item;

Item* head = NULL;

Item* find_item(const char* name) {
    for (Item* i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) return i;
    }
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "ITEM") == 0) {
            char* name = get_word(&p);
            Item* n = malloc(sizeof(Item));
            n->name = strdup(name);
            n->note = strdup(p);
            n->labels = NULL;
            n->next = head;
            head = n;
        } else if (strcmp(cmd, "LABEL") == 0) {
            char* name = get_word(&p);
            Item* i = find_item(name);
            if (i) {
                Label* l = malloc(sizeof(Label));
                l->name = strdup(p);
                l->next = i->labels;
                i->labels = l;
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char* name = get_word(&p);
            Item* i = find_item(name);
            if (i) {
                Label* prev = NULL;
                for (Label* l = i->labels; l; prev = l, l = l->next) {
                    if (strcmp(l->name, p) == 0) {
                        if (prev) prev->next = l->next;
                        else i->labels = l->next;
                        free(l->name);
                        free(l);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* name = get_word(&p);
            Item* prev = NULL;
            for (Item* i = head; i; prev = i, i = i->next) {
                if (strcmp(i->name, name) == 0) {
                    if (prev) prev->next = i->next;
                    else head = i->next;
                    free(i->name);
                    free(i->note);
                    while (i->labels) {
                        Label* temp = i->labels;
                        i->labels = i->labels->next;
                        free(temp->name);
                        free(temp);
                    }
                    free(i);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* name = get_word(&p);
            Item* i = find_item(name);
            if (i) {
                printf("%s %s", i->name, i->note);
                for (Label* l = i->labels; l; l = l->next) printf(" %s", l->name);
                printf("\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item* i = head; i; i = i->next) {
                printf("%s %s", i->name, i->note);
                for (Label* l = i->labels; l; l = l->next) printf(" %s", l->name);
                printf("\n");
            }
        }
    }
    return 0;
}