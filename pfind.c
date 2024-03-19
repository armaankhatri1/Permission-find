#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
// I pledge my Honor that I have abided by the Stevens honor system. 
// Armaan Khatri

//PermissionToString function which converts permission bits to string
void permissionsToString(mode_t mode, char str[]) {
    strcpy(str, "---------");

    if (mode & S_IRUSR) str[0] = 'r';
    if (mode & S_IWUSR) str[1] = 'w';
    if (mode & S_IXUSR) str[2] = 'x';
    if (mode & S_IRGRP) str[3] = 'r';
    if (mode & S_IWGRP) str[4] = 'w';
    if (mode & S_IXGRP) str[5] = 'x';
    if (mode & S_IROTH) str[6] = 'r';
    if (mode & S_IWOTH) str[7] = 'w';
    if (mode & S_IXOTH) str[8] = 'x';
}


// isValidPermissionsString function to validate permission string
int isValidPermissionsString(char* pstring) {
    if (strlen(pstring) != 9) {
        return 0;
    }
    for (int i = 0; i < 9; ++i) {
        if (i % 3 == 0 && pstring[i] != 'r' && pstring[i] != '-') return 0;
        if (i % 3 == 1 && pstring[i] != 'w' && pstring[i] != '-') return 0;
        if (i % 3 == 2 && pstring[i] != 'x' && pstring[i] != '-') return 0;
    }
    return 1;
}


// TraverseDirectory function recursivley traversers directories
void traverseDirectory(const char* dirPath, const char* targetPerms) {
    DIR* dir = opendir(dirPath);
    if (dir == NULL) {
        fprintf(stderr, "Error: Cannot open directory %s\n", dirPath);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
   
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

       char fullPath[PATH_MAX];
if (dirPath[strlen(dirPath) - 1] == '/') {
    snprintf(fullPath, sizeof(fullPath), "%s%s", dirPath, entry->d_name);
} else {
    snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);
}

        struct stat fileStat;
        if (stat(fullPath, &fileStat) == -1) {
            fprintf(stderr, "Error: Cannot access the file %s\n", fullPath);
            continue;
        }

        
        // checks if directory or  a file
        if (S_ISDIR(fileStat.st_mode)) {
            traverseDirectory(fullPath, targetPerms);
        } else if (S_ISREG(fileStat.st_mode)) {
            char currentPerms[10];
            permissionsToString(fileStat.st_mode, currentPerms);
            if (strcmp(currentPerms, targetPerms) == 0) {
                printf("%s\n", fullPath);
            }
        }
    }
    

    closedir(dir);
}

int main(int argc, char* argv[]) {
    char* directory = argv[1];
    char* permissionsString = argv[2];

    if (!isValidPermissionsString(permissionsString)) {
        fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", permissionsString);
        return EXIT_FAILURE;
    }

    traverseDirectory(directory, permissionsString);
    return EXIT_SUCCESS;
}
