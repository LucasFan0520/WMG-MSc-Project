// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} Rec;

Rec recs[10000];
int rc = 0;

int find_name(char *name) {
    for (int i = 0; i < rc; i++) if (strcmp(recs[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *p = rest + strlen(name);
            while (*p == ' ') p++;
            int len;
            char *colon = strchr(p, ':');
            if (colon && sscanf(p, "%d", &len) == 1) {
                if (strlen(colon + 1) == (size_t)len) {
                    if (find_name(name) == -1) {
                        recs[rc].name = strdup(name);
                        recs[rc].data = strdup(colon + 1);
                        rc++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                free(recs[idx].name);
                free(recs[idx].data);
                for (int i = idx; i < rc - 1; i++) recs[i] = recs[i+1];
                rc--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                for (char *p = recs[idx].data; *p; p++) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s\n", recs[i].name);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(recs[i].name);
        free(recs[i].data);
    }
    return 0;
}