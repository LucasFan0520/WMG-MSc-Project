// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *item; int count; } Inv;
Inv *inv = NULL;
int inv_count = 0, inv_cap = 0;

int find_item(const char *item) {
    for (int i = 0; i < inv_count; i++)
        if (strcmp(inv[i].item, item) == 0) return i;
    return -1;
}

void add_item(const char *item, int count) {
    int idx = find_item(item);
    if (idx >= 0) {
        inv[idx].count += count;
    } else {
        if (inv_count == inv_cap) {
            inv_cap = inv_cap ? inv_cap * 2 : 4;
            inv = realloc(inv, sizeof(Inv) * inv_cap);
        }
        inv[inv_count].item = strdup(item);
        inv[inv_count].count = count;
        inv_count++;
    }
}

void take_item(const char *item, int count) {
    int idx = find_item(item);
    if (idx < 0) return;
    if (count > inv[idx].count) inv[idx].count = 0;
    else inv[idx].count -= count;
}

void remove_item(const char *item) {
    int idx = find_item(item);
    if (idx < 0) return;
    free(inv[idx].item);
    for (int i = idx; i < inv_count - 1; i++) inv[i] = inv[i + 1];
    inv_count--;
}

void count_item(const char *item) {
    int idx = find_item(item);
    printf("%d\n", idx >= 0 ? inv[idx].count : 0);
}

void report_inv(void) {
    for (int i = 0; i < inv_count; i++)
        printf("%s %d\n", inv[i].item, inv[i].count);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], item[256];
        int count;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "TAKE") == 0) {
            if (sscanf(line, "%*s %255s %d", item, &count) == 2) {
                if (strcmp(cmd, "ADD") == 0) add_item(item, count);
                else take_item(item, count);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", item) == 1) remove_item(item);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", item) == 1) count_item(item);
        } else if (strcmp(cmd, "REPORT") == 0) report_inv();
    }
    for (int i = 0; i < inv_count; i++) free(inv[i].item);
    free(inv);
    return 0;
}