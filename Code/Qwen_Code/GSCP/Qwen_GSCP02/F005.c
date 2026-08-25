/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char **lines = NULL;
static int lcount = 0;
static int lcap = 0;

static int ensure_cap(void) {
    if (lcount >= lcap) {
        int newcap = lcap == 0 ? 8 : lcap * 2;
        char **tmp = realloc(lines, sizeof(char *) * newcap);
        if (!tmp) return -1;
        lines = tmp;
        lcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
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

        if (strcmp(cmd, "INSERT") == 0) {
            int index = 0;
            int off2 = 0;
            if (sscanf(line + offset, " %d%n", &index, &off2) != 1) continue;
            if (index < 0 || index > lcount) continue;
            const char *text = line + offset + off2;
            while (*text == ' ') text++;
            if (ensure_cap() != 0) continue;
            for (int i = lcount; i > index; i--)
                lines[i] = lines[i - 1];
            lines[index] = dup_str(text);
            if (!lines[index]) continue;
            lcount++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            const char *text = line + offset;
            while (*text == ' ') text++;
            if (ensure_cap() != 0) continue;
            lines[lcount] = dup_str(text);
            if (!lines[lcount]) continue;
            lcount++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            int index = 0;
            if (sscanf(line + offset, " %d", &index) != 1) continue;
            if (index < 0 || index >= lcount) continue;
            free(lines[index]);
            for (int i = index; i < lcount - 1; i++)
                lines[i] = lines[i + 1];
            lcount--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            int index = 0;
            int off2 = 0;
            if (sscanf(line + offset, " %d%n", &index, &off2) != 1) continue;
            if (index < 0 || index >= lcount) continue;
            const char *text = line + offset + off2;
            while (*text == ' ') text++;
            char *nl = dup_str(text);
            if (!nl) continue;
            free(lines[index]);
            lines[index] = nl;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < lcount; i++) {
                print_underscore(lines[i]);
            }
        }
    }
    for (int i = 0; i < lcount; i++)
        free(lines[i]);
    free(lines);
    return 0;
}