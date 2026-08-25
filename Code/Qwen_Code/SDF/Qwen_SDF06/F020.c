// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Chunk {
    char *title;
    char *body;
    struct Chunk *next;
} Chunk;

Chunk *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    char title[1024] = {0};
    char body[8192] = {0};
    int in_record = 0;
    
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            if (in_record && title[0] != '\0') {
                Chunk *c = malloc(sizeof(Chunk));
                if (c) {
                    c->title = strdup(title);
                    c->body = strdup(body);
                    c->next = head;
                    head = c;
                }
            }
            title[0] = '\0';
            body[0] = '\0';
            in_record = 1;
        } else if (in_record && title[0] == '\0') {
            strncpy(title, line, sizeof(title) - 1);
        } else if (in_record) {
            if (strlen(body) + strlen(line) + 2 < sizeof(body)) {
                if (body[0] != '\0') strcat(body, "\n");
                strcat(body, line);
            }
        }
    }
    if (in_record && title[0] != '\0') {
        Chunk *c = malloc(sizeof(Chunk));
        if (c) {
            c->title = strdup(title);
            c->body = strdup(body);
            c->next = head;
            head = c;
        }
    }
    fclose(f);
}

void find_chunk(const char *title) {
    for (Chunk *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->title, title) == 0) {
            printf("%s\n%s\n", curr->title, curr->body);
            return;
        }
    }
}

void delete_chunk(const char *title) {
    Chunk *curr = head;
    Chunk *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            Chunk *next = curr->next;
            free(curr->title);
            free(curr->body);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void add_chunk() {
    char title[1024];
    if (!fgets(title, sizeof(title), stdin)) return;
    title[strcspn(title, "\n")] = 0;
    
    char body[8192] = {0};
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) break;
        if (strlen(body) + strlen(line) + 2 < sizeof(body)) {
            if (body[0] != '\0') strcat(body, "\n");
            strcat(body, line);
        }
    }
    
    Chunk *c = malloc(sizeof(Chunk));
    if (c) {
        c->title = strdup(title);
        c->body = strdup(body);
        c->next = head;
        head = c;
    }
}

void print_chunks() {
    for (Chunk *curr = head; curr; curr = curr->next) {
        printf("%s\n%s\n", curr->title, curr->body);
    }
}

void free_all() {
    Chunk *curr = head;
    while (curr) {
        Chunk *next = curr->next;
        free(curr->title);
        free(curr->body);
        free(curr);
        curr = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char title[1024];
        
        int n = sscanf(line, "%15s %1023[^\n]", cmd, title);
        if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_chunk(title);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_chunk(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_chunk();
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_chunks();
        }
    }
    free_all();
    return 0;
}