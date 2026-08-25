// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *label; } Label;
typedef struct { char *name; char *note; Label *labels; int l_count, l_cap; } Item;
Item *items = NULL;
int item_count = 0, item_cap = 0;

int find_item(const char *name) {
    for (int i = 0; i < item_count; i++)
        if (strcmp(items[i].name, name) == 0) return i;
    return -1;
}

int find_label_in_item(Item *item, const char *label) {
    for (int i = 0; i < item->l_count; i++)
        if (strcmp(item->labels[i].label, label) == 0) return i;
    return -1;
}

void create_item(const char *name, const char *note) {
    if (find_item(name) >= 0) return;
    if (item_count == item_cap) {
        item_cap = item_cap ? item_cap * 2 : 4;
        items = realloc(items, sizeof(Item) * item_cap);
    }
    items[item_count].name = strdup(name);
    items[item_count].note = strdup(note);
    items[item_count].labels = NULL;
    items[item_count].l_count = 0;
    items[item_count].l_cap = 0;
    item_count++;
}

void add_label(const char *name, const char *label) {
    int idx = find_item(name);
    if (idx < 0) return;
    Item *it = &items[idx];
    if (find_label_in_item(it, label) >= 0) return;
    if (it->l_count == it->l_cap) {
        it->l_cap = it->l_cap ? it->l_cap * 2 : 4;
        it->labels = realloc(it->labels, sizeof(Label) * it->l_cap);
    }
    it->labels[it->l_count].label = strdup(label);
    it->l_count++;
}

void unlabel(const char *name, const char *label) {
    int idx = find_item(name);
    if (idx < 0) return;
    Item *it = &items[idx];
    int li = find_label_in_item(it, label);
    if (li < 0) return;
    free(it->labels[li].label);
    for (int i = li; i < it->l_count - 1; i++) it->labels[i] = it->labels[i + 1];
    it->l_count--;
}

void delete_item(const char *name) {
    int idx = find_item(name);
    if (idx < 0) return;
    for (int i = 0; i < items[idx].l_count; i++) free(items[idx].labels[i].label);
    free(items[idx].labels);
    free(items[idx].name);
    free(items[idx].note);
    for (int i = idx; i < item_count - 1; i++) items[i] = items[i + 1];
    item_count--;
}

void find_item_print(const char *name) {
    int idx = find_item(name);
    if (idx < 0) return;
    printf("%s %s", items[idx].name, items[idx].note);
    for (int i = 0; i < items[idx].l_count; i++)
        printf(" %s", items[idx].labels[i].label);
    putchar('\n');
}

void report_items(void) {
    for (int i = 0; i < item_count; i++) {
        printf("%s %s", items[i].name, items[i].note);
        for (int j = 0; j < items[i].l_count; j++)
            printf(" %s", items[i].labels[j].label);
        putchar('\n');
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                create_item(arg1, arg2);
        } else if (strcmp(cmd, "LABEL") == 0 || strcmp(cmd, "UNLABEL") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2) {
                if (strcmp(cmd, "LABEL") == 0) add_label(arg1, arg2);
                else unlabel(arg1, arg2);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_item_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) report_items();
    }
    for (int i = 0; i < item_count; i++) {
        for (int j = 0; j < items[i].l_count; j++) free(items[i].labels[j].label);
        free(items[i].labels);
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    return 0;
}