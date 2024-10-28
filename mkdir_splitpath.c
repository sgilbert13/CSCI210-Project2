#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

// make directory
void mkdir(char pathName[]) {
    // Check for valid path
    if (pathName == NULL || strlen(pathName) == 0 || strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64];
    char dirName[64];
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);
    
    if (parentDir == NULL) {
        printf("MKDIR ERROR: parent directory does not exist\n");
        return; // Parent directory must exist
    }

    // Iterate over existing child nodes to check for duplicates
    for (struct NODE* child = parentDir->childPtr; child != NULL; child = child->siblingPtr) {
        if (strcmp(child->name, baseName) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return; // Directory already exists
        }
    }

    // Check if the base name length is acceptable
    if (strlen(baseName) >= sizeof(baseName)) {
        printf("MKDIR ERROR: directory name too long\n");
        return;
    }

    // Create a new directory node
    struct NODE* newDir = (struct NODE*)malloc(sizeof(struct NODE));
    if (newDir == NULL) {
        perror("Failed to allocate memory");
        return; // Handle memory allocation failure
    }

    // Set up the new directory node
    strncpy(newDir->name, baseName, sizeof(newDir->name));
    newDir->name[sizeof(newDir->name) - 1] = '\0'; // Null-terminate
    newDir->fileType = 'D'; // Mark as a directory
    newDir->parentPtr = parentDir;
    newDir->childPtr = NULL;
    newDir->siblingPtr = NULL;

    // Add the new directory as the last child
    if (parentDir->childPtr == NULL) {
        parentDir->childPtr = newDir; // Set as the first child
    } else {
        struct NODE* lastChild = parentDir->childPtr;
        while (lastChild->siblingPtr != NULL) {
            lastChild = lastChild->siblingPtr; // Traverse to the last sibling
        }
        lastChild->siblingPtr = newDir; // Link new directory
    }

    // Success message
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

// handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    // Handle root directory case
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        baseName[0] = '\0'; // No base name in root
        return root; // Return root node
    }

    // Find the last slash in the pathName
    char* lastSlash = strrchr(pathName, '/');
    if (lastSlash == NULL) {
        strcpy(dirName, ""); // No directory part
        strcpy(baseName, pathName); // Everything is the base name
        return cwd; // Current working directory
    }

    // Extract directory name and base name
    size_t dirLen = lastSlash - pathName;
    strncpy(dirName, pathName, dirLen);
    dirName[dirLen] = '\0'; // Null-terminate the directory name
    strcpy(baseName, lastSlash + 1); // Base name follows the last slash

    // Start from the root node to traverse the path
    struct NODE* currentDir = root;
    char* token = strtok(dirName, "/");

    // Traverse through the directory path
    while (token != NULL) {
        struct NODE* child = currentDir->childPtr;
        int found = 0;

        // Look for the next token in the current directory's children
        while (child != NULL) {
            if (strcmp(child->name, token) == 0 && child->fileType == 'D') {
                currentDir = child; // Move down the directory structure
                found = 1;
                break; // Token found
            }
            child = child->siblingPtr; // Check next sibling
        }

        if (!found) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL; // Directory not found, return NULL
        }

        // Get the next token in the path
        token = strtok(NULL, "/");
    }

    // Return the current directory pointer
    return currentDir;
}



