// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *body; } Chunk;
Chunk *chunks = 0; int n = 0, cap = 0;

int find(char *title) {
    for (int i=0; i<n; i++) if (!strcmp(chunks[i].title, title)) return i;
    return -1;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024], title[256], body[4096];
    int reading = 0;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "--")) {
            if (reading) {
                if (n == cap) { cap = cap ? cap*2 : 4; chunks = realloc(chunks, cap*sizeof(Chunk)); }
                chunks[n].title = strdup(title); chunks[n].body = strdup(body); n++;
                reading = 0;
            } else {
                if (fgets(line, sizeof line, f)) {
                    line[strcspn(line, "\n")] = 0;
                    strcpy(title, line); body[0] = 0; reading = 1;
                }
            }
        } else if (reading) {
            if (strlen(body) + strlen(line) + 2 < sizeof body) {
                if (body[0]) strcat(body, "\n");
                strcat(body, line);
            }
        }
    }
    if (reading) {
        if (n == cap) { cap = cap ? cap*2 : 4; chunks = realloc(chunks, cap*sizeof(Chunk)); }
        chunks[n].title = strdup(title); chunks[n].body = strdup(body); n++;
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], title[256];
        sscanf(line, "%s %s", cmd, title);
        if (!strcmp(cmd, "FIND")) {
            int i = find(title);
            if (i >= 0) printf("%s\n%s\n", chunks[i].title, chunks[i].body);
        } else if (!strcmp(cmd, "DELETE")) {
            int i = find(title);
            if (i >= 0) {
                free(chunks[i].title); free(chunks[i].body);
                for (int j=i; j<n-1; j++) chunks[j] = chunks[j+1]; n--;
            }
        } else if (!strcmp(cmd, "ADD")) {
            char body[4096] = {0};
            while (fgets(line, sizeof line, stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (!strcmp(line, "--")) break;
                if (strlen(body) + strlen(line) + 2 < sizeof body) {
                    if (body[0]) strcat(body, "\n");
                    strcat(body, line);
                }
            }
            if (n == cap) { cap = cap ? cap*2 : 4; chunks = realloc(chunks, cap*sizeof(Chunk)); }
            chunks[n].title = strdup(title); chunks[n].body = strdup(body); n++;
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i=0; i<n; i++) printf("%s\n%s\n--\n", chunks[i].title, chunks[i].body);
        }
    }
    return 0;
}