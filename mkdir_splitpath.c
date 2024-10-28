#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

// Function to create a directory
void mkdir(char pathName[]) {
    // Check if the provided path is valid
    if (strcmp(pathName, "") == 0 || strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64];  // Base name for the new directory
    char dirName[64];   // Directory name for parent directory

    // Get the parent directory using splitPath
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);
    if (parentDir == NULL) return; // Exit if parent directory does not exist

    // Check if the directory already exists
    for (struct NODE* temp = parentDir->childPtr; temp != NULL; temp = temp->siblingPtr) {
        if (strcmp(temp->name, baseName) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return;
        }
    }

    // Ensure base name length is valid
    if (strlen(baseName) >= sizeof(baseName)) {
        printf("MKDIR ERROR: directory name too long\n");
        return;
    }

    // Allocate memory for the new directory node
    struct NODE* newDir = (struct NODE*)malloc(sizeof(struct NODE));
    if (newDir == NULL) {
        perror("Failed to allocate memory");
        return;
    }

    // Set the fields of the new directory node
    strncpy(newDir->name, baseName, sizeof(newDir->name) - 1);
    newDir->name[sizeof(newDir->name) - 1] = '\0'; // Null-terminate
    newDir->fileType = 'D'; // Set file type to directory
    newDir->parentPtr = parentDir;
    newDir->childPtr = NULL;
    newDir->siblingPtr = NULL;

    // Add the new directory node as the last child in the parent directory
    if (parentDir->childPtr == NULL) {
        parentDir->childPtr = newDir; // If no children, set as first child
    } else {
        struct NODE* sibling = parentDir->childPtr;
        while (sibling->siblingPtr != NULL) {
            sibling = sibling->siblingPtr; // Traverse to the last sibling
        }
        sibling->siblingPtr = newDir; // Add new directory as last sibling
    }

    // Print the success message
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

// Function to split the path into base name and directory name
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    // Handle the root case
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        baseName[0] = '\0'; // Base name is empty
        return root; // Return root as the parent directory
    }

    // Find the last occurrence of '/'
    char* lastSlash = strrchr(pathName, '/');
    if (lastSlash == NULL) {
        // If no slashes, we're in the current directory
        strcpy(dirName, "");
        strcpy(baseName, pathName);
        return cwd; // Return current working directory
    }

    // Split the directory path and base name
    size_t dirLen = lastSlash - pathName;
    strncpy(dirName, pathName, dirLen);
    dirName[dirLen] = '\0'; // Null-terminate directory name
    strcpy(baseName, lastSlash + 1); // Base name is after the last '/'

    // Traverse to find the target directory
    struct NODE* current = root;
    char* token = strtok(dirName, "/");
    while (token != NULL) {
        struct NODE* child = current->childPtr;
        int found = 0;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0 && child->fileType == 'D') {
                found = 1;
                current = child; // Navigate to the found directory
                break;
            }
            child = child->siblingPtr; // Check next sibling
        }

        if (!found) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL; // Return NULL if the directory is not found
        }
        token = strtok(NULL, "/"); // Move to the next token
    }

    return current; // Return the parent directory
}

