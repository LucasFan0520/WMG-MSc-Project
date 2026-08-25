// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LabelNode {
    char *label;
    struct LabelNode *next;
} LabelNode;

typedef struct ItemNode {
    char *name;
    char *note;
    LabelNode *labels;
    struct ItemNode *next;
} ItemNode;

ItemNode *head = NULL;

ItemNode* find_item(const char *name) {
    ItemNode *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void create_item(const char *name, const char *note) {
    if (find_item(name)) return;
    ItemNode *new = malloc(sizeof(ItemNode));
    new->name = strdup(name);
    new->note = strdup(note);
    new->labels = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        ItemNode *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void add_label(const char *name, const char *label) {
    ItemNode *item = find_item(name);
    if (!item) return;
    LabelNode *cur = item->labels;
    while (cur) {
        if (strcmp(cur->label, label) == 0) return;
        cur = cur->next;
    }
    LabelNode *new = malloc(sizeof(LabelNode));
    new->label = strdup(label);
    new->next = NULL;
    if (!item->labels) item->labels = new;
    else {
        cur = item->labels;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void remove_label(const char *name, const char *label) {
    ItemNode *item = find_item(name);
    if (!item) return;
    LabelNode *prev = NULL, *cur = item->labels;
    while (cur) {
        if (strcmp(cur->label, label) == 0) {
            if (prev) prev->next = cur->next;
            else item->labels = cur->next;
            free(cur->label);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_item(const char *name) {
    ItemNode *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur->note);
            LabelNode *l = cur->labels;
            while (l) {
                LabelNode *tmp = l;
                l = l->next;
                free(tmp->label);
                free(tmp);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_item(const char *name) {
    ItemNode *item = find_item(name);
    if (!item) return;
    printf("%s %s", item->name, item->note);
    LabelNode *l = item->labels;
    while (l) {
        printf(" %s", l->label);
        l = l->next;
    }
    printf("\n");
}

void print_all() {
    ItemNode *cur = head;
    while (cur) {
        printf("%s %s", cur->name, cur->note);
        LabelNode *l = cur->labels;
        while (l) {
            printf(" %s", l->label);
            l = l->next;
        }
        printf("\n");
        cur = cur->next;
    }
}

void free_all() {
    ItemNode *cur = head;
    while (cur) {
        ItemNode *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        LabelNode *l = tmp->labels;
        while (l) {
            LabelNode *tl = l;
            l = l->next;
            free(tl->label);
            free(tl);
        }
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            create_item(name, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *label = strtok(NULL, " ");
            if (!label) continue;
            add_label(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *label = strtok(NULL, " ");
            if (!label) continue;
            remove_label(name, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) print_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}