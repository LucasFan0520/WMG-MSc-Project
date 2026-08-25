// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *key;
    char *value;
} Var;

int main(void) {
    Var *vars = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *value = p + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(vars[found].value);
                    vars[found].value = strdup(value);
                } else {
                    if (count >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        vars = realloc(vars, cap * sizeof(Var));
                    }
                    vars[count].key = strdup(key);
                    vars[count].value = strdup(value);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') key++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(vars[found].key);
                free(vars[found].value);
                for (int i = found; i < count - 1; i++) {
                    vars[i] = vars[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            size_t out_cap = 16;
            size_t out_len = 0;
            char *out = malloc(out_cap);
            char *p = text;
            while (*p) {
                if (*p == '{') {
                    char *start = p + 1;
                    char *end = strchr(start, '}');
                    if (end) {
                        *end = '\0';
                        char *val = NULL;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, start) == 0) {
                                val = vars[i].value;
                                break;
                            }
                        }
                        if (val) {
                            size_t v_len = strlen(val);
                            while (out_len + v_len + 1 >= out_cap) {
                                out_cap *= 2;
                                out = realloc(out, out_cap);
                            }
                            strcpy(out + out_len, val);
                            out_len += v_len;
                            p = end + 1;
                        } else {
                            *end = '}';
                            while (out_len + 2 >= out_cap) {
                                out_cap *= 2;
                                out = realloc(out, out_cap);
                            }
                            out[out_len++] = *p++;
                        }
                    } else {
                        while (out_len + 2 >= out_cap) {
                            out_cap *= 2;
                            out = realloc(out, out_cap);
                        }
                        out[out_len++] = *p++;
                    }
                } else {
                    while (out_len + 2 >= out_cap) {
                        out_cap *= 2;
                        out = realloc(out, out_cap);
                    }
                    out[out_len++] = *p++;
                }
            }
            out[out_len] = '\0';
            printf("%s\n", out);
            free(out);
        } else if (strcmp(line, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
