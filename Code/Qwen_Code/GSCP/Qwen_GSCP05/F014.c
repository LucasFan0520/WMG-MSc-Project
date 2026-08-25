// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; char **labels; int lc; } Item;
Item *items = NULL; int ic = 0;

int find_item(const char *n) {
    for (int i = 0; i < ic; i++) if (strcmp(items[i].name, n) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[256];
        c2[0] = c3[0] = 0;
        sscanf(line, "%19s %255s %255[^\n]", c1, c2, c3);
        
        if (strcmp(c1, "ITEM") == 0) {
            if (find_item(c2) < 0) {
                items = realloc(items, (ic+1)*sizeof(Item));
                items[ic].name = strdup(c2); items[ic].note = strdup(c3); items[ic].labels = NULL; items[ic].lc = 0; ic++;
            }
        } else if (strcmp(c1, "LABEL") == 0) {
            int i = find_item(c2);
            if (i >= 0) {
                items[i].labels = realloc(items[i].labels, (items[i].lc+1)*sizeof(char*));
                items[i].labels[items[i].lc++] = strdup(c3);
            }
        } else if (strcmp(c1, "UNLABEL") == 0) {
            int i = find_item(c2);
            if (i >= 0) {
                for (int j = 0; j < items[i].lc; j++) {
                    if (strcmp(items[i].labels[j], c3) == 0) {
                        free(items[i].labels[j]);
                        for (int k = j; k < items[i].lc-1; k++) items[i].labels[k] = items[i].labels[k+1];
                        items[i].lc--; break;
                    }
                }
            }
        } else if (strcmp(c1, "DELETE") == 0) {
            int i = find_item(c2);
            if (i >= 0) {
                free(items[i].name); free(items[i].note);
                for (int j = 0; j < items[i].lc; j++) free(items[i].labels[j]);
                free(items[i].labels);
                for (int j = i; j < ic-1; j++) items[j] = items[j+1];
                ic--;
            }
        } else if (strcmp(c1, "FIND") == 0) {
            int i = find_item(c2);
            if (i >= 0) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].lc; j++) printf(" %s", items[i].labels[j]);
                printf("\n");
            }
        } else if (strcmp(c1, "REPORT") == 0) {
            for (int i = 0; i < ic; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].lc; j++) printf(" %s", items[i].labels[j]);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < ic; i++) {
        free(items[i].name); free(items[i].note);
        for (int j = 0; j < items[i].lc; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    free(items);
    return 0;
}