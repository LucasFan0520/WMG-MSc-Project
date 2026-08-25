// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *label; } Label;
typedef struct { char *name, *note; Label *labels; int n, cap; } Item;
Item *items = 0; int ni = 0, ci = 0;

int find_item(char *name) {
    for (int i=0; i<ni; i++) if (!strcmp(items[i].name, name)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256];
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ITEM")) sscanf(line, "%*s %s %[^\n]", a, b);
        else sscanf(line, "%*s %s %s", a, b);

        if (!strcmp(cmd, "ITEM")) {
            if (find_item(a) < 0) {
                if (ni == ci) { ci = ci ? ci*2 : 4; items = realloc(items, ci*sizeof(Item)); }
                items[ni].name = strdup(a); items[ni].note = strdup(b); items[ni].labels = 0; items[ni].n = 0; items[ni].cap = 0; ni++;
            }
        } else if (!strcmp(cmd, "LABEL")) {
            int i = find_item(a);
            if (i >= 0) {
                int exists = 0;
                for (int j=0; j<items[i].n; j++) if (!strcmp(items[i].labels[j].label, b)) { exists = 1; break; }
                if (!exists) {
                    if (items[i].n == items[i].cap) {
                        items[i].cap = items[i].cap ? items[i].cap*2 : 4;
                        items[i].labels = realloc(items[i].labels, items[i].cap*sizeof(Label));
                    }
                    items[i].labels[items[i].n++].label = strdup(b);
                }
            }
        } else if (!strcmp(cmd, "UNLABEL")) {
            int i = find_item(a);
            if (i >= 0) {
                for (int j=0; j<items[i].n; j++) {
                    if (!strcmp(items[i].labels[j].label, b)) {
                        free(items[i].labels[j].label);
                        for (int k=j; k<items[i].n-1; k++) items[i].labels[k] = items[i].labels[k+1];
                        items[i].n--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int i = find_item(a);
            if (i >= 0) {
                for (int j=0; j<items[i].n; j++) free(items[i].labels[j].label);
                free(items[i].labels); free(items[i].name); free(items[i].note);
                for (int j=i; j<ni-1; j++) items[j] = items[j+1]; ni--;
            }
        } else if (!strcmp(cmd, "FIND") || !strcmp(cmd, "REPORT")) {
            int start = (!strcmp(cmd, "FIND")) ? find_item(a) : 0;
            int end = (!strcmp(cmd, "FIND")) ? start + 1 : ni;
            for (int i=start; i<end; i++) {
                if (i >= 0 && i < ni) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (int j=0; j<items[i].n; j++) printf(" %s", items[i].labels[j].label);
                    putchar('\n');
                }
            }
        }
    }
    return 0;
}