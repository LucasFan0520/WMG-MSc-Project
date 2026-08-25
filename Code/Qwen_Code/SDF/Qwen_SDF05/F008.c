// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *name; long score; char *note; } Student;
static Student *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Student *t = realloc(arr, nc * sizeof(Student));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_student(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void cmd_add(char *name, long score, char *note) {
    if (find_student(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].score = score;
    arr[cnt].note = safe_strdup(note);
    if (!arr[cnt].name || !arr[cnt].note) {
        free(arr[cnt].name); free(arr[cnt].note);
        arr[cnt].name = NULL; arr[cnt].note = NULL;
        return;
    }
    cnt++;
}

static void cmd_update(char *name, long score, char *note) {
    int idx = find_student(name);
    if (idx < 0) return;
    char *nn = safe_strdup(note);
    if (!nn) return;
    free(arr[idx].note);
    arr[idx].score = score;
    arr[idx].note = nn;
}

static void cmd_drop(char *name) {
    int idx = find_student(name);
    if (idx < 0) return;
    free(arr[idx].name); free(arr[idx].note);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_average(void) {
    if (cnt == 0) { printf("0\n"); return; }
    long sum = 0;
    for (size_t i = 0; i < cnt; i++) sum += arr[i].score;
    printf("%ld\n", sum / (long)cnt);
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %ld %s\n", arr[i].name, arr[i].score, arr[i].note);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].name); free(arr[i].note); }
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char name[256] = {0}; long score = 0; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%255s%n %ld%n", name, &n2, &score, &n3) < 2) continue;
            char *note = rest + n3;
            while (*note == ' ') note++;
            if (strcmp(cmd, "ADD") == 0) cmd_add(name, score, note);
            else cmd_update(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_drop(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            cmd_average();
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}