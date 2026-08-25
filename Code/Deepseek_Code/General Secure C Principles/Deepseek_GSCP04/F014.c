/* F014.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name;
    char *note;
    char **labels;
    size_t lcount, lcap;
} Item;

static Item *items = NULL;
static size_t icount = 0, icap = 0;

static Item *find_item(const char *name) {
    for (size_t i = 0; i < icount; i++) {
        if (strcmp(items[i].name, name) == 0)
            return &items[i];
    }
    return NULL;
}

static void add_item(const char *name, const char *note) {
    if (icount == icap) {
        icap = icap ? icap * 2 : 4;
        items = realloc(items, icap * sizeof(Item));
        if (!items) { perror("realloc"); exit(1); }
    }
    items[icount].name = strdup(name);
    items[icount].note = strdup(note);
    items[icount].labels = NULL;
    items[icount].lcount = items[icount].lcap = 0;
    icount++;
}

static void add_label(Item *item, const char *label) {
    if (item->lcount == item->lcap) {
        item->lcap = item->lcap ? item->lcap * 2 : 4;
        item->labels = realloc(item->labels, item->lcap * sizeof(char *));
        if (!item->labels) { perror("realloc"); exit(1); }
    }
    item->labels[item->lcount] = strdup(label);
    item->lcount++;
}

static void remove_label(Item *item, const char *label) {
    for (size_t i = 0; i < item->lcount; i++) {
        if (strcmp(item->labels[i], label) == 0) {
            free(item->labels[i]);
            for (size_t j = i; j < item->lcount - 1; j++)
                item->labels[j] = item->labels[j + 1];
            item->lcount--;
            return;
        }
    }
}

static void delete_item(const char *name) {
    for (size_t i = 0; i < icount; i++) {
        if (strcmp(items[i].name, name) == 0) {
            free(items[i].name);
            free(items[i].note);
            for (size_t j = 0; j < items[i].lcount; j++) free(items[i].labels[j]);
            free(items[i].labels);
            for (size_t j = i; j < icount - 1; j++)
                items[j] = items[j + 1];
            icount--;
            return;
        }
    }
}

static void find_item_print(const char *name) {
    Item *item = find_item(name);
    if (!item) return;
    printf("%s %s", item->name, item->note);
    for (size_t i = 0; i < item->lcount; i++) {
        printf(" %s", item->labels[i]);
    }
    printf("\n");
}

static void report_items(void) {
    for (size_t i = 0; i < icount; i++) {
        printf("%s %s", items[i].name, items[i].note);
        for (size_t j = 0; j < items[i].lcount; j++) {
            printf(" %s", items[i].labels[j]);
        }
        printf("\n");
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            if (find_item(name)) {
                // update note? Spec says create item; if exists, maybe replace note? We'll update note.
                Item *it = find_item(name);
                free(it->note);
                it->note = strdup(note);
            } else {
                add_item(name, note);
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (!name || !label) continue;
            Item *it = find_item(name);
            if (it) add_label(it, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (!name || !label) continue;
            Item *it = find_item(name);
            if (it) remove_label(it, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    for (size_t i = 0; i < icount; i++) {
        free(items[i].name);
        free(items[i].note);
        for (size_t j = 0; j < items[i].lcount; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    free(items);
    return 0;
}