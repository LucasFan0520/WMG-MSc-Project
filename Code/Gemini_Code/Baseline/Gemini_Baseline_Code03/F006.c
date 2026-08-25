// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InventoryItem;

int main() {
    InventoryItem *inv = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "TAKE") == 0) {
            char item[256];
            int val = 0;
            if (sscanf(ptr, "%255s %d", item, &val) == 2) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        found = i;
                        break;
                    }
                }
                if (strcmp(cmd, "ADD") == 0) {
                    if (found != -1) {
                        inv[found].count += val;
                    } else {
                        inv = realloc(inv, (count + 1) * sizeof(InventoryItem));
                        inv[count].item = strdup(item);
                        inv[count].count = val;
                        count++;
                    }
                } else {
                    if (found != -1) {
                        inv[found].count -= val;
                        if (inv[found].count < 0) {
                            inv[found].count = 0;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char item[256];
            if (sscanf(ptr, "%255s", item) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(inv[found].item);
                    for (int i = found; i < count - 1; i++) {
                        inv[i] = inv[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        inv = realloc(inv, count * sizeof(InventoryItem));
                    } else {
                        free(inv);
                        inv = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char item[256];
            if (sscanf(ptr, "%255s", item) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%d\n", inv[found].count);
                } else {
                    printf("0\n");
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
