// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; char* note; } Note;

Note* store = NULL;
size_t s_count = 0, s_cap = 0;

void cleanup_store(Note** arr, size_t* count) {
    for (size_t i = 0; i < *count; i++) { free((*arr)[i].name); free((*arr)[i].note); }
    free(*arr);
    *arr = NULL;
    *count = 0;
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    char* line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "LOAD") == 0) {
            FILE* f = fopen(argv[1], "r");
            if (!f) continue;
            
            Note* temp_store = NULL;
            size_t t_count = 0, t_cap = 0;
            int valid = 1;
            
            char* fline = NULL;
            size_t flen = 0;
            while (getline(&fline, &flen, f) != -1) {
                if (fline[strlen(fline) - 1] == '\n') fline[strlen(fline) - 1] = '\0';
                char* p = fline;
                while (*p && isspace((unsigned char)*p)) p++;
                char* name = p;
                while (*p && !isspace((unsigned char)*p)) p++;
                if (*p) { *p = '\0'; p++; }
                while (*p && isspace((unsigned char)*p)) p++;
                char* note = p;
                
                if (!*name) { valid = 0; break; }
                
                if (t_count == t_cap) {
                    size_t new_cap = t_cap == 0 ? 4 : t_cap * 2;
                    Note* temp = realloc(temp_store, new_cap * sizeof(Note));
                    if (!temp) { valid = 0; break; }
                    temp_store = temp; t_cap = new_cap;
                }
                temp_store[t_count].name = strdup(name);
                temp_store[t_count].note = strdup(note);
                if (!temp_store[t_count].name || !temp_store[t_count].note) { valid = 0; break; }
                t_count++;
            }
            free(fline);
            fclose(f);
            
            if (valid) {
                cleanup_store(&store, &s_count);
                store = temp_store;
                s_count = t_count;
                s_cap = t_cap;
            } else {
                cleanup_store(&temp_store, &t_count);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;

            if (s_count == s_cap) {
                size_t new_cap = s_cap == 0 ? 4 : s_cap * 2;
                Note* temp = realloc(store, new_cap * sizeof(Note));
                if (!temp) { cleanup_store(&store, &s_count); free(line); return 1; }
                store = temp; s_cap = new_cap;
            }
            store[s_count].name = strdup(name);
            store[s_count].note = strdup(note);
            if (!store[s_count].name || !store[s_count].note) { cleanup_store(&store, &s_count); free(line); return 1; }
            s_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < s_count; i++) {
                if (strcmp(store[i].name, ptr) == 0) {
                    free(store[i].name); free(store[i].note);
                    for (size_t j = i; j < s_count - 1; j++) store[j] = store[j + 1];
                    s_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < s_count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
    }
    free(line);
    cleanup_store(&store, &s_count);
    return 0;
}