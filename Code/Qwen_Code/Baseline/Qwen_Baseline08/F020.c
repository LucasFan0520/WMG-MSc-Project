// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 1000
#define MAX_LINES 100
#define MAX_LEN 1000

typedef struct {
    char title[100];
    char body[MAX_LINES][MAX_LEN];
    int lcount;
} Record;

Record recs[MAX_RECORDS];
int rcount = 0;

int find_rec(const char *title) {
    for (int i = 0; i < rcount; i++) if (strcmp(recs[i].title, title) == 0) return i;
    return -1;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (f) {
        char line[MAX_LEN];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, "--") == 0) continue;
            if (rcount < MAX_RECORDS) {
                strcpy(recs[rcount].title, line);
                recs[rcount].lcount = 0;
                while (fgets(line, sizeof(line), f)) {
                    line[strcspn(line, "\n")] = 0;
                    if (strcmp(line, "--") == 0) break;
                    if (recs[rcount].lcount < MAX_LINES) {
                        strcpy(recs[rcount].body[recs[rcount].lcount++], line);
                    }
                }
                rcount++;
            }
        }
        fclose(f);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char cmd[20], title[100], line[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx = find_rec(title);
            if (idx != -1) {
                printf("%s\n", recs[idx].title);
                for (int i = 0; i < recs[idx].lcount; i++) printf("%s\n", recs[idx].body[i]);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", title);
            int idx = find_rec(title);
            if (idx != -1) {
                for (int i = idx; i < rcount - 1; i++) recs[i] = recs[i + 1];
                rcount--;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", title);
            if (rcount < MAX_RECORDS) {
                strcpy(recs[rcount].title, title);
                recs[rcount].lcount = 0;
                while (fgets(line, sizeof(line), stdin)) {
                    line[strcspn(line, "\n")] = 0;
                    if (strcmp(line, "--") == 0) break;
                    if (recs[rcount].lcount < MAX_LINES) {
                        strcpy(recs[rcount].body[recs[rcount].lcount++], line);
                    }
                }
                rcount++;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", recs[i].title);
                for (int j = 0; j < recs[i].lcount; j++) printf("%s\n", recs[i].body[j]);
                printf("--\n");
            }
        }
    }
    return 0;
}