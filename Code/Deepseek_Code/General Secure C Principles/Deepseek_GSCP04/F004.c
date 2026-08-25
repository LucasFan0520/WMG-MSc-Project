/* F004.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Var;

static Var *vars = NULL;
static size_t vcount = 0, vcap = 0;

static Var *find_var(const char *key) {
    for (size_t i = 0; i < vcount; i++) {
        if (strcmp(vars[i].key, key) == 0)
            return &vars[i];
    }
    return NULL;
}

static void set_var(const char *key, const char *value) {
    Var *v = find_var(key);
    if (v) {
        free(v->value);
        v->value = strdup(value);
    } else {
        if (vcount == vcap) {
            vcap = vcap ? vcap * 2 : 4;
            vars = realloc(vars, vcap * sizeof(Var));
            if (!vars) { perror("realloc"); exit(1); }
        }
        vars[vcount].key = strdup(key);
        vars[vcount].value = strdup(value);
        vcount++;
    }
}

static void unset_var(const char *key) {
    for (size_t i = 0; i < vcount; i++) {
        if (strcmp(vars[i].key, key) == 0) {
            free(vars[i].key);
            free(vars[i].value);
            for (size_t j = i; j < vcount - 1; j++)
                vars[j] = vars[j + 1];
            vcount--;
            return;
        }
    }
}

static void render_text(const char *text) {
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *start = p + 1;
            const char *end = strchr(start, '}');
            if (end) {
                size_t klen = end - start;
                char *key = malloc(klen + 1);
                if (!key) { perror("malloc"); exit(1); }
                memcpy(key, start, klen);
                key[klen] = '\0';
                Var *v = find_var(key);
                if (v) {
                    fputs(v->value, stdout);
                } else {
                    putchar('{');
                    fwrite(start, 1, klen, stdout);
                    putchar('}');
                }
                free(key);
                p = end + 1;
                continue;
            }
        }
        putchar(*p);
        p++;
    }
    putchar('\n');
}

static void vars_list(void) {
    for (size_t i = 0; i < vcount; i++) {
        printf("%s %s\n", vars[i].key, vars[i].value);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            set_var(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = strtok(NULL, "");
            if (!text) text = "";
            render_text(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_list();
        }
    }
    for (size_t i = 0; i < vcount; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}