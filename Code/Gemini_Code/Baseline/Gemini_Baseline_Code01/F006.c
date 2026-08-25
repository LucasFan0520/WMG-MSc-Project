// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InvItem;

int main() {
    InvItem *inv = NULL;
    int total_items = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int amt = 0;
            if (sscanf(args, "%255s %d", name, &amt) == 2) {
                int found = 0;
                for (int i = 0; i < total_items; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        inv[i].count += amt;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    inv = realloc(inv, (total_items + 1) * sizeof(InvItem));
                    inv[total_items].item = strdup(name);
                    inv[total_items].count = amt;
                    total_items++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char name[256];
            int amt = 0;
            if (sscanf(args, "%255s %d", name, &amt) == 2) {
                for (int i = 0; i < total_items; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        inv[i].count -= amt;
                        if (inv[i].count < 0) {
                            inv[i].count = 0;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < total_items; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        free(inv[i].item);
                        for (int j = i; j < total_items - 1; j++) {
                            inv[j] = inv[j + 1];
                        }
                        total_items--;
                        inv = realloc(inv, total_items * sizeof(InvItem));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                int c = 0;
                for (int i = 0; i < total_items; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        c = inv[i].count;
                        break;
                    }
                }
                printf("%d\n", c);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < total_items; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
    }
    for (int i = 0; i < total_items; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
