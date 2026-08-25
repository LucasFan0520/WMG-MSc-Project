// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *ingredient; char *note; } Ing;
typedef struct { char *name; Ing *ings; int n; } Rec;
Rec *recs = NULL;
int nr = 0;

int find_rec(const char *name) {
    for (int i = 0; i < nr; i++) if (strcmp(recs[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "RECIPE") == 0) {
            sscanf(line, "%*s %s", a);
            if (find_rec(a) == -1) {
                recs = realloc(recs, (nr + 1) * sizeof(Rec));
                recs[nr].name = strdup(a);
                recs[nr].ings = NULL;
                recs[nr].n = 0;
                nr++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            char *note = strchr(t, ' ');
            char ing_name[256];
            strncpy(ing_name, t, note ? note - t : strlen(t));
            ing_name[note ? note - t : strlen(t)] = 0;
            
            int idx = find_rec(a);
            if (idx != -1) {
                recs[idx].ings = realloc(recs[idx].ings, (recs[idx].n + 1) * sizeof(Ing));
                recs[idx].ings[recs[idx].n].ingredient = strdup(ing_name);
                recs[idx].ings[recs[idx].n].note = strdup(note ? note + 1 : "");
                recs[idx].n++;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            int idx = find_rec(a);
            if (idx != -1) {
                for (int i = 0; i < recs[idx].n; i++) {
                    if (strcmp(recs[idx].ings[i].ingredient, b) == 0) {
                        free(recs[idx].ings[i].ingredient);
                        free(recs[idx].ings[i].note);
                        for (int j = i; j < recs[idx].n - 1; j++) recs[idx].ings[j] = recs[idx].ings[j + 1];
                        recs[idx].n--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            sscanf(line, "%*s %s", a);
            int idx = find_rec(a);
            if (idx != -1) {
                for (int i = 0; i < recs[idx].n; i++) {
                    free(recs[idx].ings[i].ingredient);
                    free(recs[idx].ings[i].note);
                }
                free(recs[idx].ings);
                free(recs[idx].name);
                for (int i = idx; i < nr - 1; i++) recs[i] = recs[i + 1];
                nr--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            sscanf(line, "%*s %s", a);
            int idx = find_rec(a);
            if (idx != -1) {
                printf("%s:\n", recs[idx].name);
                for (int i = 0; i < recs[idx].n; i++) {
                    printf("  %s %s\n", recs[idx].ings[i].ingredient, recs[idx].ings[i].note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < nr; i++) printf("%s\n", recs[i].name);
        }
    }
    for (int i = 0; i < nr; i++) {
        for (int j = 0; j < recs[i].n; j++) {
            free(recs[i].ings[j].ingredient);
            free(recs[i].ings[j].note);
        }
        free(recs[i].ings);
        free(recs[i].name);
    }
    free(recs);
    return 0;
}