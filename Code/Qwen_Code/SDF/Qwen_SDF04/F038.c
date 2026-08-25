// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; char* encoded; } Encoded;

Encoded* encs = NULL;
size_t e_count = 0, e_cap = 0;

void cleanup() {
    for (size_t i = 0; i < e_count; i++) { free(encs[i].name); free(encs[i].encoded); }
    free(encs);
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

        if (strcmp(cmd, "STORE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* enc = ptr;

            if (e_count == e_cap) {
                size_t new_cap = e_cap == 0 ? 4 : e_cap * 2;
                Encoded* temp = realloc(encs, new_cap * sizeof(Encoded));
                if (!temp) { cleanup(); free(line); return 1; }
                encs = temp; e_cap = new_cap;
            }
            encs[e_count].name = strdup(name);
            encs[e_count].encoded = strdup(enc);
            if (!encs[e_count].name || !encs[e_count].encoded) { cleanup(); free(line); return 1; }
            e_count++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < e_count; i++) {
                if (strcmp(encs[i].name, ptr) == 0) {
                    char* p = encs[i].encoded;
                    int valid = 1;
                    while (*p) {
                        long count = 0;
                        while (isdigit((unsigned char)*p)) {
                            count = count * 10 + (*p - '0');
                            if (count > 100000) { valid = 0; break; }
                            p++;
                        }
                        if (!valid || !*p) break;
                        p++;
                    }
                    if (valid) {
                        p = encs[i].encoded;
                        while (*p) {
                            long count = 0;
                            while (isdigit((unsigned char)*p)) {
                                count = count * 10 + (*p - '0');
                                p++;
                            }
                            char c = *p++;
                            for (long j = 0; j < count; j++) {
                                putchar(c == ' ' ? '_' : c);
                            }
                        }
                        putchar('\n');
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < e_count; i++) {
                if (strcmp(encs[i].name, ptr) == 0) {
                    free(encs[i].name); free(encs[i].encoded);
                    for (size_t j = i; j < e_count - 1; j++) encs[j] = encs[j + 1];
                    e_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < e_count; i++) {
                printf("%s\n", encs[i].name);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}