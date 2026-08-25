// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int bcount;
} Chunk;

Chunk *chunks = NULL;
int ccount = 0;

int find_chunk(const char *title) {
    for (int i = 0; i < ccount; i++) {
        if (strcmp(chunks[i].title, title) == 0) return i;
    }
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1000];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) continue;
                chunks = realloc(chunks, sizeof(Chunk) * (ccount + 1));
                chunks[ccount].title = strdup(line);
                chunks[ccount].body = NULL;
                chunks[ccount].bcount = 0;
                while (fgets(line, sizeof(line), f)) {
                    line[strcspn(line, "\n")] = 0;
                    if (strcmp(line, "--") == 0) break;
                    chunks[ccount].body = realloc(chunks[ccount].body, sizeof(char*) * (chunks[ccount].bcount + 1));
                    chunks[ccount].body[chunks[ccount].bcount++] = strdup(line);
                }
                ccount++;
            }
            fclose(f);
        }
    }
    
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "FIND") == 0) {
            char title[100];
            scanf(" %[^\n]", title);
            int idx = find_chunk(title);
            if (idx != -1) {
                printf("%s\n", chunks[idx].title);
                for (int i = 0; i < chunks[idx].bcount; i++) {
                    printf("%s\n", chunks[idx].body[i]);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[100];
            scanf(" %[^\n]", title);
            int idx = find_chunk(title);
            if (idx != -1) {
                free(chunks[idx].title);
                for (int i = 0; i < chunks[idx].bcount; i++) free(chunks[idx].body[i]);
                free(chunks[idx].body);
                for (int i = idx; i < ccount - 1; i++) chunks[i] = chunks[i+1];
                ccount--;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char title[100];
            scanf(" %[^\n]", title);
            chunks = realloc(chunks, sizeof(Chunk) * (ccount + 1));
            chunks[ccount].title = strdup(title);
            chunks[ccount].body = NULL;
            chunks[ccount].bcount = 0;
            char line[1000];
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                chunks[ccount].body = realloc(chunks[ccount].body, sizeof(char*) * (chunks[ccount].bcount + 1));
                chunks[ccount].body[chunks[ccount].bcount++] = strdup(line);
            }
            ccount++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s\n", chunks[i].title);
                for (int j = 0; j < chunks[i].bcount; j++) {
                    printf("%s\n", chunks[i].body[j]);
                }
                printf("--\n");
            }
        }
    }
    for (int i = 0; i < ccount; i++) {
        free(chunks[i].title);
        for (int j = 0; j < chunks[i].bcount; j++) free(chunks[i].body[j]);
        free(chunks[i].body);
    }
    free(chunks);
    return 0;
}