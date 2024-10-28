
#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

// Function to create a directory
void mkdir(char pathName[]) {
    // Check if the provided path name is valid
    if (strcmp(pathName, "") == 0 || strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: No path provided\n");
        return;
    }

    char baseName[64];  // Base name of the new directory
    char dirName[64];   // Directory name to hold parent directory path

    // Split the provided path into base name and directory name
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);
    
    // If the parent directory doesn't exist, abort
    if (parentDir == NULL) {
        return;
    }

    // Check if the directory already exists
    for (struct NODE* temp = parentDir->childPtr; temp != NULL; temp = temp->siblingPtr) {
        if (strcmp(temp->name, baseName) == 0) {
            printf("MKDIR ERROR: Directory %s already exists\n", pathName);
            return;
        }
    }

    // Ensure the base name length is valid
    if (strlen(baseName) >= sizeof(baseName)) {
        printf("MKDIR ERROR: Directory name too long\n");
        return;
    }

    // Allocate memory for the new directory node
    struct NODE* newDir = (struct NODE*)malloc(sizeof(struct NODE));
    if (newDir == NULL) {
        perror("Failed to allocate memory");
        return;
    }

    // Set fields of the new directory node
    strncpy(newDir->name, baseName, sizeof(newDir->name) - 1);
    newDir->name[sizeof(newDir->name) - 1] = '\0'; // Null-terminate
    newDir->fileType = 'D'; // 'D' denotes a directory
    newDir->parentPtr = parentDir;
    newDir->childPtr = NULL;
    newDir->siblingPtr = NULL;

    // Add the new directory node as the last child in the parent directory
    if (parentDir->childPtr == NULL) {
        parentDir->childPtr = newDir; // Set as first child if none exists
    } else {
        struct NODE* sibling = parentDir->childPtr;
        while (sibling->siblingPtr != NULL) {
            sibling = sibling->siblingPtr; // Traverse to the last sibling
        }
        sibling->siblingPtr = newDir; // Add as a sibling
    }

    // Print success message
    printf("MKDIR SUCCESS: Node %s successfully created\n", pathName);
}

// Function to split the path into base name and directory name
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    // Handle the case where the path is the root
    if (strcmp(pathName, "/") == 0) {
        baseName[0] = '\0'; // Base name is empty
        strcpy(dirName, "/");
        return root; // Return root as the parent directory
    }

    // Find the last occurrence of the '/' character
    char* lastSlash = strrchr(pathName, '/');
    if (lastSlash == NULL) {
        // If no slash, we're in the current directory
        strcpy(dirName, "");
        strcpy(baseName, pathName);
        return cwd; // Return current working directory
    }

    // Separate the directory path and base name
    size_t dirLen = lastSlash - pathName;
    strncpy(dirName, pathName, dirLen);
    dirName[dirLen] = '\0'; // Null-terminate directory name
    strcpy(baseName, lastSlash + 1); // Base name is after the last '/'

    // Traverse to find the target directory
    struct NODE* current = root;
    char* token = strtok(dirName, "/"); // Tokenize the directory path
    while (token != NULL) {
        struct NODE* child = current->childPtr;
        int found = 0;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0 && child->fileType == 'D') {
                current = child; // Navigate to the found directory
                found = 1;
                break;
            }
            child = child->siblingPtr; // Check next sibling
        }

        if (!found) {
            printf("ERROR: Directory %s does not exist\n", token);
            return NULL; // Return NULL if the directory is not found
        }
        token = strtok(NULL, "/"); // Move to the next token
    }

    return current; // Return the parent directory
}
