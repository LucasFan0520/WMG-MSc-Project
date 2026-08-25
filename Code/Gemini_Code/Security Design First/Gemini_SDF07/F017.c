/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigKV;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    ConfigKV *cfg = NULL;
    int count = 0;
    int capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread_bytes;
        while ((fread_bytes = getline(&fline, &flen, fp)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            char *eq = strchr(fline, '=');
            if (!eq) continue;
            *eq = '\0';
            char *k = fline;
            char *v = eq + 1;
            char *n_key = strdup(k);
            char *n_val = strdup(v);
            if (n_key && n_val) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    ConfigKV *tmp = realloc(cfg, new_cap * sizeof(ConfigKV));
                    if (tmp) {
                        cfg = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_key); free(n_val);
                        continue;
                    }
                }
                cfg[count].key = n_key;
                cfg[count].value = n_val;
                count++;
            } else {
                free(n_key); free(n_val);
            }
        }
        free(fline);
        fclose(fp);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            char *k = NULL;
            char *v = NULL;
            if (space) {
                *space = '\0';
                k = args;
                v = space + 1;
            } else {
                k = args;
                v = "";
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, k) == 0) {
                    char *n_val = strdup(v);
                    if (n_val) {
                        free(cfg[i].value);
                        cfg[i].value = n_val;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                char *n_key = strdup(k);
                char *n_val = strdup(v);
                if (n_key && n_val) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        ConfigKV *tmp = realloc(cfg, new_cap * sizeof(ConfigKV));
                        if (tmp) {
                            cfg = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_key); free(n_val);
                            if (space) *space = ' ';
                            continue;
                        }
                    }
                    cfg[count].key = n_key;
                    cfg[count].value = n_val;
                    count++;
                } else {
                    free(n_key); free(n_val);
                }
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *k = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, k) == 0) {
                    free(cfg[i].key);
                    free(cfg[i].value);
                    for (int j = i; j < count - 1; j++) {
                        cfg[j] = cfg[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *k = line + 4;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, k) == 0) {
                    printf("%s\n", cfg[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", cfg[i].key, cfg[i].value);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(cfg[i].key);
        free(cfg[i].value);
    }
    free(cfg);
    return 0;
}
