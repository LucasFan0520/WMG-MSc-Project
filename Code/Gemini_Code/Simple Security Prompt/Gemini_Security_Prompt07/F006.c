// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InvItem;

int main() {
    InvItem *items = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *item = strtok(NULL, " ");
            char *cnt_s = strtok(NULL, " ");
            if (item && cnt_s) {
                int cnt = atoi(cnt_s);
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].item, item) == 0) {
                        items[i].count += cnt;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    items = realloc(items, (count + 1) * sizeof(InvItem));
                    items[count].item = strdup(item);
                    items[count].count = cnt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *item = strtok(NULL, " ");
            char *cnt_s = strtok(NULL, " ");
            if (item && cnt_s) {
                int cnt = atoi(cnt_s);
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].item, item) == 0) {
                        items[i].count -= cnt;
                        if (items[i].count < 0) {
                            items[i].count = 0;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = strtok(NULL, " ");
            if (item) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].item, item) == 0) {
                        free(items[i].item);
                        for (int j = i; j < count - 1; j++) {
                            items[j] = items[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(items);
                    items = NULL;
                } else {
                    items = realloc(items, count * sizeof(InvItem));
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = strtok(NULL, " ");
            if (item) {
                int cnt = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].item, item) == 0) {
                        cnt = items[i].count;
                        break;
                    }
                }
                printf("%d\n", cnt);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", items[i].item, items[i].count);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(items[i].item);
    }
    free(items);
    return 0;
}
