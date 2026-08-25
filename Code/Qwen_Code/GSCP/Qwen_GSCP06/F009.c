// F009.c
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

typedef struct Desc {
    char* text;
    struct Desc* next;
} Desc;

typedef struct Group {
    char* name;
    Desc* descs;
    struct Group* next;
} Group;

Group* head = NULL;

Group* find_group(const char* name) {
    for (Group* g = head; g; g = g->next) {
        if (strcmp(g->name, name) == 0) return g;
    }
    return NULL;
}

void add_desc(Group* g, const char* text) {
    Desc* d = malloc(sizeof(Desc));
    d->text = strdup(text);
    d->next = NULL;
    if (!g->descs) g->descs = d;
    else {
        Desc* tail = g->descs;
        while (tail->next) tail = tail->next;
        tail->next = d;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "EVENT") == 0) {
            char* gname = get_word(&p);
            Group* g = find_group(gname);
            if (!g) {
                g = malloc(sizeof(Group));
                g->name = strdup(gname);
                g->descs = NULL;
                g->next = head;
                head = g;
            }
            add_desc(g, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char* gA = get_word(&p);
            char* gB = get_word(&p);
            char* newG = p;
            Group* n = malloc(sizeof(Group));
            n->name = strdup(newG);
            n->descs = NULL;
            n->next = head;
            head = n;
            
            Group* a = find_group(gA);
            if (a) for (Desc* d = a->descs; d; d = d->next) add_desc(n, d->text);
            Group* b = find_group(gB);
            if (b) for (Desc* d = b->descs; d; d = d->next) add_desc(n, d->text);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char* gname = get_word(&p);
            Group* prev = NULL;
            for (Group* g = head; g; prev = g, g = g->next) {
                if (strcmp(g->name, gname) == 0) {
                    if (prev) prev->next = g->next;
                    else head = g->next;
                    free(g->name);
                    while (g->descs) {
                        Desc* temp = g->descs;
                        g->descs = g->descs->next;
                        free(temp->text);
                        free(temp);
                    }
                    free(g);
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char* gname = get_word(&p);
            Group* g = find_group(gname);
            int c = 0;
            if (g) for (Desc* d = g->descs; d; d = d->next) c++;
            printf("%d\n", c);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Group* g = head; g; g = g->next) {
                for (Desc* d = g->descs; d; d = d->next) {
                    printf("%s %s\n", g->name, d->text);
                }
            }
        }
    }
    return 0;
}