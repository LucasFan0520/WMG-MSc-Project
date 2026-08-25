/* F014.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int lcount;
    int lcap;
} Item;

static Item *items = NULL;
static int icount = 0;
static int icap = 0;

static int ensure_icap(void) {
    if (icount >= icap) {
        int newcap = icap == 0 ? 8 : icap * 2;
        Item *tmp = realloc(items, sizeof(Item) * newcap);
        if (!tmp) return -1;
        items = tmp;
        icap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_item(const char *name) {
    for (int i = 0; i < icount; i++) {
        if (strcmp(items[i].name, name) == 0)
            return i;
    }
    return -1;
}

static int item_add_label(Item *it, const char *label) {
    if (it->lcount >= it->lcap) {
        int newcap = it->lcap == 0 ? 4 : it->lcap * 2;
        char **tmp = realloc(it->labels, sizeof(char *) * newcap);
        if (!tmp) return -1;
        it->labels = tmp;
        it->lcap = newcap;
    }
    it->labels[it->lcount] = dup_str(label);
    if (!it->labels[it->lcount]) return -1;
    it->lcount++;
    return 0;
}

static void free_item_data(Item *it) {
    free(it->name);
    free(it->note);
    for (int i = 0; i < it->lcount; i++)
        free(it->labels[i]);
    free(it->labels);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ITEM") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", name, &off2) != 1) continue;
            const char *note = line + offset + off2;
            while (*note == ' ') note++;
            if (find_item(name) >= 0) continue;
            if (ensure_icap() != 0) continue;
            items[icount].name = dup_str(name);
            items[icount].note = dup_str(note);
            items[icount].labels = NULL;
            items[icount].lcount = 0;
            items[icount].lcap = 0;
            if (!items[icount].name || !items[icount].note) {
                free(items[icount].name);
                free(items[icount].note);
                continue;
            }
            icount++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256] = {0}, label[256] = {0};
            if (sscanf(line + offset, " %255s %255s", name, label) != 2) continue;
            int idx = find_item(name);
            if (idx < 0) continue;
            item_add_label(&items[idx], label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256] = {0}, label[256] = {0};
            if (sscanf(line + offset, " %255s %255s", name, label) != 2) continue;
            int idx = find_item(name);
            if (idx < 0) continue;
            for (int i = 0; i < items[idx].lcount; i++) {
                if (strcmp(items[idx].labels[i], label) == 0) {
                    free(items[idx].labels[i]);
                    for (int j = i; j < items[idx].lcount - 1; j++)
                        items[idx].labels[j] = items[idx].labels[j + 1];
                    items[idx].lcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_item(name);
            if (idx < 0) continue;
            free_item_data(&items[idx]);
            for (int i = idx; i < icount - 1; i++)
                items[i] = items[i + 1];
            icount--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_item(name);
            if (idx >= 0) {
                printf("%s %s", items[idx].name, items[idx].note);
                for (int i = 0; i < items[idx].lcount; i++)
                    printf(" [%s]", items[idx].labels[i]);
                printf("\n");
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < icount; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].lcount; j++)
                    printf(" [%s]", items[i].labels[j]);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < icount; i++)
        free_item_data(&items[i]);
    free(items);
    return 0;
}