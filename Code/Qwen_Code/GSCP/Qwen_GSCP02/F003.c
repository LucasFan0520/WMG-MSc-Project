/* F003.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

static Alias *aliases = NULL;
static int acount = 0;
static int acap = 0;

static int ensure_cap(void) {
    if (acount >= acap) {
        int newcap = acap == 0 ? 8 : acap * 2;
        Alias *tmp = realloc(aliases, sizeof(Alias) * newcap);
        if (!tmp) return -1;
        aliases = tmp;
        acap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_alias(const char *name) {
    for (int i = 0; i < acount; i++) {
        if (strcmp(aliases[i].name, name) == 0)
            return i;
    }
    return -1;
}

static void print_underscore(const char *s) {
    for (size_t i = 0; s[i]; i++)
        putchar(s[i] == ' ' ? '_' : s[i]);
    putchar('\n');
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", name, &off2) != 1) continue;
            const char *exp = line + offset + off2;
            while (*exp == ' ') exp++;
            int idx = find_alias(name);
            if (idx >= 0) {
                char *ne = dup_str(exp);
                if (!ne) continue;
                free(aliases[idx].expansion);
                aliases[idx].expansion = ne;
            } else {
                if (ensure_cap() != 0) continue;
                aliases[acount].name = dup_str(name);
                aliases[acount].expansion = dup_str(exp);
                if (!aliases[acount].name || !aliases[acount].expansion) {
                    free(aliases[acount].name);
                    free(aliases[acount].expansion);
                    continue;
                }
                acount++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_alias(name);
            if (idx >= 0) {
                print_underscore(aliases[idx].expansion);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char oldn[256] = {0}, newn[256] = {0};
            if (sscanf(line + offset, " %255s %255s", oldn, newn) != 2) continue;
            int idx = find_alias(oldn);
            if (idx < 0) continue;
            if (find_alias(newn) >= 0) continue;
            char *nn = dup_str(newn);
            if (!nn) continue;
            free(aliases[idx].name);
            aliases[idx].name = nn;
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_alias(name);
            if (idx < 0) continue;
            free(aliases[idx].name);
            free(aliases[idx].expansion);
            for (int i = idx; i < acount - 1; i++)
                aliases[i] = aliases[i + 1];
            acount--;
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < acount; i++) {
                printf("%s ", aliases[i].name);
                print_underscore(aliases[i].expansion);
            }
        }
    }
    for (int i = 0; i < acount; i++) {
        free(aliases[i].name);
        free(aliases[i].expansion);
    }
    free(aliases);
    return 0;
}