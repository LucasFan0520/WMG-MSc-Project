// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main(void) {
    RLEString *store = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char name[256], enc[4096];
            if (sscanf(line + 6, "%255s %4095s", name, enc) == 2) {
                RLEString *tmp = realloc(store, (count + 1) * sizeof(RLEString));
                if (tmp) {
                    store = tmp;
                    store[count].name = strdup(name);
                    store[count].encoded = strdup(enc);
                    count++;
                }
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    char *p = store[i].encoded;
                    int ok = 1;
                    while (*p && ok) {
                        int cnt = 0;
                        if (*p >= '0' && *p <= '9') {
                            while (*p >= '0' && *p <= '9') {
                                cnt = cnt * 10 + (*p - '0');
                                if (cnt > 10000) {
                                    ok = 0;
                                    break;
                                }
                                p++;
                            }
                            if (*p && ok) {
                                char c = *p;
                                if (c == ' ') c = '_';
                                for (int k = 0; k < cnt; k++) {
                                    putchar(c);
                                }
                                p++;
                            } else {
                                ok = 0;
                            }
                        } else {
                            ok = 0;
                        }
                    }
                    if (ok) {
                        putchar('\n');
                    } else {
                        printf("INVALID\n");
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].encoded);
                    for (int j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}
