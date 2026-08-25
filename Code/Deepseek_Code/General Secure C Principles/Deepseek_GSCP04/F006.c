/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} Inventory;

static Inventory *inv = NULL;
static size_t icount = 0, icap = 0;

static Inventory *find_item(const char *item) {
    for (size_t i = 0; i < icount; i++) {
        if (strcmp(inv[i].item, item) == 0)
            return &inv[i];
    }
    return NULL;
}

static void add_item(const char *item, int cnt) {
    Inventory *p = find_item(item);
    if (p) {
        p->count += cnt;
    } else {
        if (icount == icap) {
            icap = icap ? icap * 2 : 4;
            inv = realloc(inv, icap * sizeof(Inventory));
            if (!inv) { perror("realloc"); exit(1); }
        }
        inv[icount].item = strdup(item);
        inv[icount].count = cnt;
        icount++;
    }
}

static void take_item(const char *item, int cnt) {
    Inventory *p = find_item(item);
    if (!p) return;
    if (p->count < cnt)
        p->count = 0;
    else
        p->count -= cnt;
}

static void remove_item(const char *item) {
    for (size_t i = 0; i < icount; i++) {
        if (strcmp(inv[i].item, item) == 0) {
            free(inv[i].item);
            for (size_t j = i; j < icount - 1; j++)
                inv[j] = inv[j + 1];
            icount--;
            return;
        }
    }
}

static void count_item(const char *item) {
    Inventory *p = find_item(item);
    printf("%d\n", p ? p->count : 0);
}

static void report_items(void) {
    for (size_t i = 0; i < icount; i++) {
        printf("%s %d\n", inv[i].item, inv[i].count);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *item = strtok(NULL, " ");
            char *cntstr = strtok(NULL, " ");
            if (!item || !cntstr) continue;
            int cnt = atoi(cntstr);
            add_item(item, cnt);
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *item = strtok(NULL, " ");
            char *cntstr = strtok(NULL, " ");
            if (!item || !cntstr) continue;
            int cnt = atoi(cntstr);
            take_item(item, cnt);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = strtok(NULL, " ");
            if (item) remove_item(item);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = strtok(NULL, " ");
            if (item) count_item(item);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    for (size_t i = 0; i < icount; i++)
        free(inv[i].item);
    free(inv);
    return 0;
}