#include "types.h"

extern struct NODE* root; // Pointer to the root of the file system
extern struct NODE* cwd;  // Pointer to the current working directory

// Function to make a directory
void mkdir(char pathName[]) {
    // Check for a valid path
    if (pathName == NULL || strlen(pathName) == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64];
    char dirName[64];

    // Use splitPath to get the base name and parent directory
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);
    
    if (parentDir == NULL) {
        printf("MKDIR ERROR: parent directory does not exist\n");
        return; // Parent directory must exist
    }

    // Check if the directory already exists
    for (struct NODE* child = parentDir->childPtr; child != NULL; child = child->siblingPtr) {
        if (strcmp(child->name, baseName) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return; // Directory already exists
        }
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

// Function to change the current directory
void cd(char* pathName) {
    struct NODE* targetDir = splitPath(pathName, NULL, NULL);
    if (targetDir == NULL) {
        printf("CD ERROR: directory %s does not exist\n", pathName);
    } else {
        cwd = targetDir; // Change the current working directory
    }
}

// Function to display the directory structure
void treeHelper(struct NODE* node, int depth) {
    if (node == NULL) return;

    // Indentation for the current depth
    for (int i = 0; i < depth; i++) {
        printf("   ");
    }
    printf("|---%s\n", node->name); // Print current directory name

    // Recursive call for child nodes
    treeHelper(node->childPtr, depth + 1); // Child directories

    // Recursive call for sibling nodes
    treeHelper(node->siblingPtr, depth); // Sibling directories
}

// Function to print the tree starting from the current directory
void tree() {
    treeHelper(cwd, 0); // Start from the current working directory
}

// Handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    // Handle root directory case
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        baseName[0] = '\0'; // No base name in root
        return root; // Return root node
    }

    // Determine if path is absolute or relative
    int isAbsolute = (pathName[0] == '/');
    struct NODE* currentDir = isAbsolute ? root : cwd; // Start from root or cwd

    // Find the last slash in the pathName
    char* lastSlash = strrchr(pathName, '/');
    if (lastSlash == NULL) {
        strcpy(dirName, ""); // No directory part
        strcpy(baseName, pathName); // Everything is the base name
        return currentDir; // Current working directory
    }

    // Extract directory name and base name
    size_t dirLen = lastSlash - pathName;
    strncpy(dirName, pathName, dirLen);
    dirName[dirLen] = '\0'; // Null-terminate the directory name
    strcpy(baseName, lastSlash + 1); // Base name follows the last slash

    // Traverse through the directory path
    char* token = strtok(dirName, "/");
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
