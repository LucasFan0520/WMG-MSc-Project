// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Chunk;

Chunk chunks[10000];
int cc = 0;

int find_title(char *title) {
    for (int i = 0; i < cc; i++) if (strcmp(chunks[i].title, title) == 0) return i;
    return -1;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    char title[1000] = "";
    char body[100000] = "";
    int in_body = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            if (title[0]) {
                chunks[cc].title = strdup(title);
                chunks[cc].body = strdup(body);
                cc++;
            }
            title[0] = 0;
            body[0] = 0;
            in_body = 0;
        } else {
            if (!in_body) {
                strcpy(title, line);
                in_body = 1;
            } else {
                if (body[0]) strcat(body, "\n");
                strcat(body, line);
            }
        }
    }
    if (title[0]) {
        chunks[cc].title = strdup(title);
        chunks[cc].body = strdup(body);
        cc++;
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "FIND") == 0) {
            int idx = find_title(rest);
            if (idx != -1) printf("%s\n%s\n", chunks[idx].title, chunks[idx].body);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_title(rest);
            if (idx != -1) {
                free(chunks[idx].title); free(chunks[idx].body);
                for (int i = idx; i < cc - 1; i++) chunks[i] = chunks[i+1];
                cc--;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            chunks[cc].title = strdup(rest);
            char body[100000] = "";
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                if (body[0]) strcat(body, "\n");
                strcat(body, line);
            }
            chunks[cc].body = strdup(body);
            cc++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < cc; i++) {
                printf("%s\n%s\n--\n", chunks[i].title, chunks[i].body);
            }
        }
    }
    for (int i = 0; i < cc; i++) {
        free(chunks[i].title); free(chunks[i].body);
    }
    return 0;
}