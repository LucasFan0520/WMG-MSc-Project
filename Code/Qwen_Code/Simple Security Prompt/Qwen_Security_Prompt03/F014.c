// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; char **labels; int lcnt; } Item;
Item *items = NULL;
int n = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], name[256], val[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ITEM") == 0) {
            sscanf(line, "%*s %s", name);
            char *note = strchr(line, ' '); note++; note = strchr(note, ' ');
            items = realloc(items, (n + 1) * sizeof(Item));
            items[n].name = strdup(name); items[n].note = strdup(note ? note + 1 : ""); items[n].labels = NULL; items[n].lcnt = 0; n++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            sscanf(line, "%*s %s %s", name, val);
            for (int i = 0; i < n; i++) if (strcmp(items[i].name, name) == 0) {
                items[i].labels = realloc(items[i].labels, (items[i].lcnt + 1) * sizeof(char*));
                items[i].labels[items[i].lcnt++] = strdup(val); break;
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            sscanf(line, "%*s %s %s", name, val);
            for (int i = 0; i < n; i++) if (strcmp(items[i].name, name) == 0) {
                for (int j = 0; j < items[i].lcnt; j++) if (strcmp(items[i].labels[j], val) == 0) {
                    free(items[i].labels[j]);
                    for (int x = j; x < items[i].lcnt - 1; x++) items[i].labels[x] = items[i].labels[x + 1];
                    items[i].lcnt--; break;
                }
                break;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", name);
            for (int i = 0; i < n; i++) if (strcmp(items[i].name, name) == 0) {
                free(items[i].name); free(items[i].note);
                for (int j = 0; j < items[i].lcnt; j++) free(items[i].labels[j]);
                free(items[i].labels);
                for (int j = i; j < n - 1; j++) items[j] = items[j + 1];
                n--; break;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", name);
            for (int i = 0; i < n; i++) if (strcmp(items[i].name, name) == 0) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].lcnt; j++) printf(" %s", items[i].labels[j]);
                printf("\n"); break;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].lcnt; j++) printf(" %s", items[i].labels[j]);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < n; i++) {
        free(items[i].name); free(items[i].note);
        for (int j = 0; j < items[i].lcnt; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    free(items);
    return 0;
}