// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main() {
    RLEString *store = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "STORE") == 0) {
            char name[256], enc[1024];
            if (sscanf(ptr, "%255s %1023s", name, enc) == 2) {
                store = realloc(store, (count + 1) * sizeof(RLEString));
                store[count].name = strdup(name);
                store[count].encoded = strdup(enc);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    char *t = store[found].encoded;
                    int valid = 1;
                    while (*t && valid) {
                        if (isdigit(*t)) {
                            long run = 0;
                            while (*t && isdigit(*t)) {
                                run = run * 10 + (*t - '0');
                                if (run > 10000) {
                                    valid = 0;
                                    break;
                                }
                                t++;
                            }
                            if (*t && valid) {
                                char ch = *t;
                                for (long j = 0; j < run; j++) {
                                    if (ch == ' ') putchar('_');
                                    else putchar(ch);
                                }
                                t++;
                            } else {
                                valid = 0;
                            }
                        } else {
                            valid = 0;
                        }
                    }
                    if (!valid) {
                        printf("INVALID");
                    }
                    putchar('\n');
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(store[found].name);
                    free(store[found].encoded);
                    for (int i = found; i < count - 1; i++) {
                        store[i] = store[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        store = realloc(store, count * sizeof(RLEString));
                    } else {
                        free(store);
                        store = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}
