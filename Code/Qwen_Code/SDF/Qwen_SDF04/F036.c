// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; char* data; } Record;

Record* records = NULL;
size_t r_count = 0, r_cap = 0;

void cleanup() {
    for (size_t i = 0; i < r_count; i++) { free(records[i].name); free(records[i].data); }
    free(records);
}

int main() {
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

        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            
            char* colon = strchr(ptr, ':');
            if (!colon) continue;
            *colon = '\0';
            long expected_len = strtol(ptr, NULL, 10);
            char* data = colon + 1;
            if ((long)strlen(data) != expected_len) continue;

            if (r_count == r_cap) {
                size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                Record* temp = realloc(records, new_cap * sizeof(Record));
                if (!temp) { cleanup(); free(line); return 1; }
                records = temp; r_cap = new_cap;
            }
            records[r_count].name = strdup(name);
            records[r_count].data = strdup(data);
            if (!records[r_count].name || !records[r_count].data) { cleanup(); free(line); return 1; }
            r_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].name, ptr) == 0) {
                    free(records[i].name); free(records[i].data);
                    for (size_t j = i; j < r_count - 1; j++) records[j] = records[j + 1];
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].name, ptr) == 0) {
                    for (size_t j = 0; records[i].data[j]; j++) {
                        putchar(records[i].data[j] == ' ' ? '_' : records[i].data[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}