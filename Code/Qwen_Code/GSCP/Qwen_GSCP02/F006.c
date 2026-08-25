/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InvItem;

static InvItem *items = NULL;
static int icount = 0;
static int icap = 0;

static int ensure_cap(void) {
    if (icount >= icap) {
        int newcap = icap == 0 ? 8 : icap * 2;
        InvItem *tmp = realloc(items, sizeof(InvItem) * newcap);
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
        if (strcmp(items[i].item, name) == 0)
            return i;
    }
    return -1;
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

        if (strcmp(cmd, "ADD") == 0) {
            char name[256] = {0};
            int cnt = 0;
            if (sscanf(line + offset, " %255s %d", name, &cnt) != 2) continue;
            if (cnt < 0) continue;
            int idx = find_item(name);
            if (idx >= 0) {
                items[idx].count += cnt;
            } else {
                if (ensure_cap() != 0) continue;
                items[icount].item = dup_str(name);
                if (!items[icount].item) continue;
                items[icount].count = cnt;
                icount++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char name[256] = {0};
            int cnt = 0;
            if (sscanf(line + offset, " %255s %d", name, &cnt) != 2) continue;
            if (cnt < 0) continue;
            int idx = find_item(name);
            if (idx < 0) continue;
            items[idx].count -= cnt;
            if (items[idx].count < 0) items[idx].count = 0;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_item(name);
            if (idx < 0) continue;
            free(items[idx].item);
            for (int i = idx; i < icount - 1; i++)
                items[i] = items[i + 1];
            icount--;
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_item(name);
            if (idx >= 0) {
                printf("%d\n", items[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < icount; i++) {
                printf("%s %d\n", items[i].item, items[i].count);
            }
        }
    }
    for (int i = 0; i < icount; i++)
        free(items[i].item);
    free(items);
    return 0;
}