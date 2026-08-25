// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *label;
} Label;

typedef struct {
    char *name;
    char *note;
    Label *labels;
    int nl;
} Item;

Item *items = NULL;
int n = 0;

int find_item(const char *name) {
    for (int i = 0; i < n; i++)
        if (strcmp(items[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000], arg2[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *p = line + 5 + strlen(arg1);
            if (*p == ' ') p++;
            items = realloc(items, sizeof(Item) * (n + 1));
            items[n].name = strdup(arg1);
            items[n].note = strdup(p);
            items[n].labels = NULL;
            items[n].nl = 0;
            n++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            sscanf(line, "%*s %s %s", arg1, arg2);
            int i = find_item(arg1);
            if (i != -1) {
                items[i].labels = realloc(items[i].labels, sizeof(Label) * (items[i].nl + 1));
                items[i].labels[items[i].nl++].label = strdup(arg2);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            sscanf(line, "%*s %s %s", arg1, arg2);
            int i = find_item(arg1);
            if (i != -1) {
                for (int j = 0; j < items[i].nl; j++) {
                    if (strcmp(items[i].labels[j].label, arg2) == 0) {
                        free(items[i].labels[j].label);
                        for (int k = j; k < items[i].nl - 1; k++) items[i].labels[k] = items[i].labels[k + 1];
                        items[i].nl--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_item(arg1);
            if (i != -1) {
                free(items[i].name);
                free(items[i].note);
                for (int j = 0; j < items[i].nl; j++) free(items[i].labels[j].label);
                free(items[i].labels);
                for (int j = i; j < n - 1; j++) items[j] = items[j + 1];
                n--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_item(arg1);
            if (i != -1) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].nl; j++) printf(" %s", items[i].labels[j].label);
                printf("\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].nl; j++) printf(" %s", items[i].labels[j].label);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < n; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].nl; j++) free(items[i].labels[j].label);
        free(items[i].labels);
    }
    free(items);
    return 0;
}