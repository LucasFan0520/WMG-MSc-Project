// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* sender; char* subject; char* body; char** tags; size_t tag_count, tag_cap; } Mail;

Mail* mails = NULL;
size_t m_count = 0, m_cap = 0;

void cleanup() {
    for (size_t i = 0; i < m_count; i++) {
        for (size_t j = 0; j < mails[i].tag_count; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
    }
    free(mails);
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

        if (strcmp(cmd, "MAIL") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* sender = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* subject = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* body = ptr;

            if (m_count == m_cap) {
                size_t new_cap = m_cap == 0 ? 4 : m_cap * 2;
                Mail* temp = realloc(mails, new_cap * sizeof(Mail));
                if (!temp) { cleanup(); free(line); return 1; }
                mails = temp; m_cap = new_cap;
            }
            mails[m_count].sender = strdup(sender);
            mails[m_count].subject = strdup(subject);
            mails[m_count].body = strdup(body);
            mails[m_count].tags = NULL;
            mails[m_count].tag_count = 0;
            mails[m_count].tag_cap = 0;
            if (!mails[m_count].sender || !mails[m_count].subject || !mails[m_count].body) {
                cleanup(); free(line); return 1;
            }
            m_count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* subject = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* tag = ptr;

            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    if (mails[i].tag_count == mails[i].tag_cap) {
                        size_t new_cap = mails[i].tag_cap == 0 ? 4 : mails[i].tag_cap * 2;
                        char** temp = realloc(mails[i].tags, new_cap * sizeof(char*));
                        if (!temp) { cleanup(); free(line); return 1; }
                        mails[i].tags = temp; mails[i].tag_cap = new_cap;
                    }
                    mails[i].tags[mails[i].tag_count] = strdup(tag);
                    if (!mails[i].tags[mails[i].tag_count]) { cleanup(); free(line); return 1; }
                    mails[i].tag_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* subject = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* tag = ptr;

            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    for (size_t j = 0; j < mails[i].tag_count; j++) {
                        if (strcmp(mails[i].tags[j], tag) == 0) {
                            free(mails[i].tags[j]);
                            for (size_t k = j; k < mails[i].tag_count - 1; k++) mails[i].tags[k] = mails[i].tags[k + 1];
                            mails[i].tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, ptr) == 0) {
                    for (size_t j = 0; j < mails[i].tag_count; j++) free(mails[i].tags[j]);
                    free(mails[i].tags);
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (size_t j = i; j < m_count - 1; j++) mails[j] = mails[j + 1];
                    m_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < m_count; i++) {
                for (size_t j = 0; j < mails[i].tag_count; j++) {
                    if (strcmp(mails[i].tags[j], ptr) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < m_count; i++) {
                printf("%s %s %s\n", mails[i].sender, mails[i].subject, mails[i].body);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}