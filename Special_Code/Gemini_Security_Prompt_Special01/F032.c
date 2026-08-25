// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *key;
    char *value;
} Setting;

int main(void) {
    Setting *main_store = NULL;
    size_t main_count = 0;
    size_t main_capacity = 0;
    
    Setting *stage_store = NULL;
    size_t stage_count = 0;
    size_t stage_capacity = 0;
    
    int tx = 0;
    
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            char *val = key_end;
            if (*key_end != '\0') {
                *key_end = '\0';
                val++;
            }
            while (*val == ' ') val++;
            int found = 0;
            for (size_t i = 0; i < main_count; i++) {
                if (strcmp(main_store[i].key, key) == 0) {
                    free(main_store[i].value);
                    main_store[i].value = mystrdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (main_count >= main_capacity) {
                    main_capacity = main_capacity == 0 ? 4 : main_capacity * 2;
                    Setting *new_m = realloc(main_store, main_capacity * sizeof(Setting));
                    if (new_m) main_store = new_m;
                }
                main_store[main_count].key = mystrdup(key);
                main_store[main_count].value = mystrdup(val);
                main_count++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            tx = 1;
            for (size_t i = 0; i < stage_count; i++) {
                free(stage_store[i].key);
                free(stage_store[i].value);
            }
            stage_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (tx) {
                char *key = p;
                while (*key == ' ') key++;
                char *key_end = key;
                while (*key_end && *key_end != ' ') key_end++;
                char *val = key_end;
                if (*key_end != '\0') {
                    *key_end = '\0';
                    val++;
                }
                while (*val == ' ') val++;
                int found = 0;
                for (size_t i = 0; i < stage_count; i++) {
                    if (strcmp(stage_store[i].key, key) == 0) {
                        free(stage_store[i].value);
                        stage_store[i].value = mystrdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (stage_count >= stage_capacity) {
                        stage_capacity = stage_capacity == 0 ? 4 : stage_capacity * 2;
                        Setting *new_s = realloc(stage_store, stage_capacity * sizeof(Setting));
                        if (new_s) stage_store = new_s;
                    }
                    stage_store[stage_count].key = mystrdup(key);
                    stage_store[stage_count].value = mystrdup(val);
                    stage_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (tx) {
                for (size_t i = 0; i < stage_count; i++) {
                    int found = 0;
                    for (size_t j = 0; j < main_count; j++) {
                        if (strcmp(main_store[j].key, stage_store[i].key) == 0) {
                            free(main_store[j].value);
                            main_store[j].value = mystrdup(stage_store[i].value);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (main_count >= main_capacity) {
                            main_capacity = main_capacity == 0 ? 4 : main_capacity * 2;
                            Setting *new_m = realloc(main_store, main_capacity * sizeof(Setting));
                            if (new_m) main_store = new_m;
                        }
                        main_store[main_count].key = mystrdup(stage_store[i].key);
                        main_store[main_count].value = mystrdup(stage_store[i].value);
                        main_count++;
                    }
                }
                for (size_t i = 0; i < stage_count; i++) {
                    free(stage_store[i].key);
                    free(stage_store[i].value);
                }
                stage_count = 0;
                tx = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (tx) {
                for (size_t i = 0; i < stage_count; i++) {
                    free(stage_store[i].key);
                    free(stage_store[i].value);
                }
                stage_count = 0;
                tx = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            *key_end = '\0';
            int printed = 0;
            if (tx) {
                for (size_t i = 0; i < stage_count; i++) {
                    if (strcmp(stage_store[i].key, key) == 0) {
                        printf("%s\n", stage_store[i].value);
                        printed = 1;
                        break;
                    }
                }
            }
            if (!printed) {
                for (size_t i = 0; i < main_count; i++) {
                    if (strcmp(main_store[i].key, key) == 0) {
                        printf("%s\n", main_store[i].value);
                        printed = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < main_count; i++) {
                printf("%s %s\n", main_store[i].key, main_store[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < main_count; i++) {
        free(main_store[i].key);
        free(main_store[i].value);
    }
    free(main_store);
    for (size_t i = 0; i < stage_count; i++) {
        free(stage_store[i].key);
        free(stage_store[i].value);
    }
    free(stage_store);
    return 0;
}
