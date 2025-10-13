/**
 * CompileOS Simple Filesystem - Implementation
 * 
 * In-memory filesystem with basic file operations
 */

#include "fs.h"
#include "../memory/memory.h"
#include <string.h>

// File entry structure
typedef struct {
    bool in_use;
    char name[FS_MAX_FILENAME];
    fs_file_type_t type;
    uint8_t* data;
    size_t size;
    size_t capacity;
    uint8_t permissions;
    uint32_t created_time;
    uint32_t modified_time;
} fs_file_entry_t;

// File descriptor structure
typedef struct {
    bool in_use;
    fs_file_entry_t* file;
    size_t position;
    int flags;
} fs_descriptor_t;

// Filesystem state
static struct {
    bool initialized;
    fs_file_entry_t files[FS_MAX_FILES];
    fs_descriptor_t descriptors[FS_MAX_FILES];
    size_t file_count;
} g_fs = {0};

// Initialize filesystem
int fs_init(void) {
    if (g_fs.initialized) {
        return 0;
    }
    
    memset(&g_fs, 0, sizeof(g_fs));
    g_fs.initialized = true;
    g_fs.file_count = 0;
    
    // Create root directory
    fs_mkdir("/");
    
    // Prepopulate with some sample files (acts like a tiny initrd)
    fs_fd_t fdr = fs_open("/README.txt", FS_O_WRONLY | FS_O_CREATE | FS_O_TRUNC);
    if (fdr >= 0) {
        const char* txt = "Welcome to CompileOS\nUse 'help', 'ls', 'cat /README.txt'\n";
        fs_write(fdr, txt, (int)strlen(txt));
        fs_close(fdr);
    }
    fs_fd_t fds = fs_open("/hello.txt", FS_O_WRONLY | FS_O_CREATE | FS_O_TRUNC);
    if (fds >= 0) {
        const char* txt = "Hello from the in-memory filesystem!\n";
        fs_write(fds, txt, (int)strlen(txt));
        fs_close(fds);
    }
    
    return 0;
}

// Shutdown filesystem
void fs_shutdown(void) {
    // Free all file data
    for (size_t i = 0; i < FS_MAX_FILES; i++) {
        if (g_fs.files[i].in_use && g_fs.files[i].data) {
            memory_free(g_fs.files[i].data);
        }
    }
    
    g_fs.initialized = false;
}

// Check if initialized
bool fs_is_initialized(void) {
    return g_fs.initialized;
}

// Find file by path
static fs_file_entry_t* fs_find_file(const char* path) {
    if (!path) {
        return NULL;
    }
    
    for (size_t i = 0; i < FS_MAX_FILES; i++) {
        if (g_fs.files[i].in_use && strcmp(g_fs.files[i].name, path) == 0) {
            return &g_fs.files[i];
        }
    }
    
    return NULL;
}

// Allocate file entry
static fs_file_entry_t* fs_alloc_file(void) {
    for (size_t i = 0; i < FS_MAX_FILES; i++) {
        if (!g_fs.files[i].in_use) {
            memset(&g_fs.files[i], 0, sizeof(fs_file_entry_t));
            g_fs.files[i].in_use = true;
            g_fs.file_count++;
            return &g_fs.files[i];
        }
    }
    
    return NULL;
}

// Allocate file descriptor
static fs_fd_t fs_alloc_descriptor(fs_file_entry_t* file, int flags) {
    for (size_t i = 0; i < FS_MAX_FILES; i++) {
        if (!g_fs.descriptors[i].in_use) {
            g_fs.descriptors[i].in_use = true;
            g_fs.descriptors[i].file = file;
            g_fs.descriptors[i].position = 0;
            g_fs.descriptors[i].flags = flags;
            return (fs_fd_t)i;
        }
    }
    
    return -1;
}

// Create file
int fs_create(const char* path) {
    if (!g_fs.initialized || !path) {
        return -1;
    }
    
    // Check if file already exists
    if (fs_find_file(path)) {
        return -1; // File exists
    }
    
    // Allocate file entry
    fs_file_entry_t* file = fs_alloc_file();
    if (!file) {
        return -1; // No space
    }
    
    // Initialize file
    strncpy(file->name, path, FS_MAX_FILENAME - 1);
    file->name[FS_MAX_FILENAME - 1] = '\0';
    file->type = FS_TYPE_FILE;
    file->data = NULL;
    file->size = 0;
    file->capacity = 0;
    file->permissions = FS_PERM_READ | FS_PERM_WRITE;
    file->created_time = 0; // TODO: Get real time
    file->modified_time = 0;
    
    return 0;
}

// Open file
fs_fd_t fs_open(const char* path, int flags) {
    if (!g_fs.initialized || !path) {
        return -1;
    }
    
    // Find or create file
    fs_file_entry_t* file = fs_find_file(path);
    
    if (!file) {
        if (flags & FS_O_CREATE) {
            if (fs_create(path) != 0) {
                return -1;
            }
            file = fs_find_file(path);
        } else {
            return -1; // File not found
        }
    }
    
    // Truncate if requested
    if (flags & FS_O_TRUNC) {
        file->size = 0;
    }
    
    // Allocate descriptor
    fs_fd_t fd = fs_alloc_descriptor(file, flags);
    
    // Set position for append mode
    if (flags & FS_O_APPEND) {
        g_fs.descriptors[fd].position = file->size;
    }
    
    return fd;
}

// Close file
int fs_close(fs_fd_t fd) {
    if (!g_fs.initialized || fd < 0 || fd >= FS_MAX_FILES) {
        return -1;
    }
    
    if (!g_fs.descriptors[fd].in_use) {
        return -1;
    }
    
    g_fs.descriptors[fd].in_use = false;
    return 0;
}

// Read from file
int fs_read(fs_fd_t fd, void* buffer, size_t size) {
    if (!g_fs.initialized || fd < 0 || fd >= FS_MAX_FILES || !buffer) {
        return -1;
    }
    
    fs_descriptor_t* desc = &g_fs.descriptors[fd];
    if (!desc->in_use || !desc->file) {
        return -1;
    }
    
    // Check permissions
    if (!(desc->flags & (FS_O_RDONLY | FS_O_RDWR))) {
        return -1;
    }
    
    // Calculate bytes to read
    size_t available = desc->file->size - desc->position;
    size_t to_read = (size < available) ? size : available;
    
    if (to_read > 0 && desc->file->data) {
        memcpy(buffer, desc->file->data + desc->position, to_read);
        desc->position += to_read;
    }
    
    return to_read;
}

// Write to file
int fs_write(fs_fd_t fd, const void* buffer, size_t size) {
    if (!g_fs.initialized || fd < 0 || fd >= FS_MAX_FILES || !buffer) {
        return -1;
    }
    
    fs_descriptor_t* desc = &g_fs.descriptors[fd];
    if (!desc->in_use || !desc->file) {
        return -1;
    }
    
    // Check permissions
    if (!(desc->flags & (FS_O_WRONLY | FS_O_RDWR))) {
        return -1;
    }
    
    fs_file_entry_t* file = desc->file;
    
    // Check if we need to expand the file
    size_t required_size = desc->position + size;
    if (required_size > file->capacity) {
        // Allocate new buffer
        size_t new_capacity = required_size + 1024; // Add some extra space
        if (new_capacity > FS_MAX_FILE_SIZE) {
            new_capacity = FS_MAX_FILE_SIZE;
        }
        
        uint8_t* new_data = (uint8_t*)memory_alloc(new_capacity);
        if (!new_data) {
            return -1; // Out of memory
        }
        
        // Copy old data
        if (file->data) {
            memcpy(new_data, file->data, file->size);
            memory_free(file->data);
        }
        
        file->data = new_data;
        file->capacity = new_capacity;
    }
    
    // Write data
    memcpy(file->data + desc->position, buffer, size);
    desc->position += size;
    
    // Update file size
    if (desc->position > file->size) {
        file->size = desc->position;
    }
    
    file->modified_time = 0; // TODO: Get real time
    
    return size;
}

// Seek in file
int fs_seek(fs_fd_t fd, int offset, int whence) {
    if (!g_fs.initialized || fd < 0 || fd >= FS_MAX_FILES) {
        return -1;
    }
    
    fs_descriptor_t* desc = &g_fs.descriptors[fd];
    if (!desc->in_use || !desc->file) {
        return -1;
    }
    
    size_t new_pos;
    
    switch (whence) {
        case FS_SEEK_SET:
            new_pos = offset;
            break;
        case FS_SEEK_CUR:
            new_pos = desc->position + offset;
            break;
        case FS_SEEK_END:
            new_pos = desc->file->size + offset;
            break;
        default:
            return -1;
    }
    
    if (new_pos > desc->file->size) {
        return -1;
    }
    
    desc->position = new_pos;
    return 0;
}

// Get current position
int fs_tell(fs_fd_t fd) {
    if (!g_fs.initialized || fd < 0 || fd >= FS_MAX_FILES) {
        return -1;
    }
    
    fs_descriptor_t* desc = &g_fs.descriptors[fd];
    if (!desc->in_use) {
        return -1;
    }
    
    return desc->position;
}

// Delete file
int fs_delete(const char* path) {
    if (!g_fs.initialized || !path) {
        return -1;
    }
    
    fs_file_entry_t* file = fs_find_file(path);
    if (!file) {
        return -1;
    }
    
    // Free data
    if (file->data) {
        memory_free(file->data);
    }
    
    file->in_use = false;
    g_fs.file_count--;
    
    return 0;
}

// Check if file exists
int fs_exists(const char* path) {
    return fs_find_file(path) != NULL ? 1 : 0;
}

// Get file info
int fs_get_info(const char* path, fs_file_info_t* info) {
    if (!g_fs.initialized || !path || !info) {
        return -1;
    }
    
    fs_file_entry_t* file = fs_find_file(path);
    if (!file) {
        return -1;
    }
    
    strncpy(info->name, file->name, FS_MAX_FILENAME - 1);
    info->name[FS_MAX_FILENAME - 1] = '\0';
    info->type = file->type;
    info->size = file->size;
    info->permissions = file->permissions;
    info->created_time = file->created_time;
    info->modified_time = file->modified_time;
    
    return 0;
}

// Create directory
int fs_mkdir(const char* path) {
    if (!g_fs.initialized || !path) {
        return -1;
    }
    
    // Check if already exists
    if (fs_find_file(path)) {
        return -1;
    }
    
    // Allocate file entry
    fs_file_entry_t* dir = fs_alloc_file();
    if (!dir) {
        return -1;
    }
    
    // Initialize directory
    strncpy(dir->name, path, FS_MAX_FILENAME - 1);
    dir->name[FS_MAX_FILENAME - 1] = '\0';
    dir->type = FS_TYPE_DIRECTORY;
    dir->permissions = FS_PERM_READ | FS_PERM_WRITE | FS_PERM_EXECUTE;
    
    return 0;
}

// List directory
int fs_list(const char* path, fs_file_info_t* files, size_t max_files, size_t* count) {
    if (!g_fs.initialized || !path || !files || !count) {
        return -1;
    }
    
    *count = 0;
    
    // List all files (simple implementation - no directory hierarchy)
    for (size_t i = 0; i < FS_MAX_FILES && *count < max_files; i++) {
        if (g_fs.files[i].in_use) {
            strncpy(files[*count].name, g_fs.files[i].name, FS_MAX_FILENAME - 1);
            files[*count].name[FS_MAX_FILENAME - 1] = '\0';
            files[*count].type = g_fs.files[i].type;
            files[*count].size = g_fs.files[i].size;
            files[*count].permissions = g_fs.files[i].permissions;
            files[*count].created_time = g_fs.files[i].created_time;
            files[*count].modified_time = g_fs.files[i].modified_time;
            (*count)++;
        }
    }
    
    return 0;
}

// Get file size
int fs_get_size(const char* path) {
    fs_file_entry_t* file = fs_find_file(path);
    if (!file) {
        return -1;
    }
    return file->size;
}

// Rename file
int fs_rename(const char* old_path, const char* new_path) {
    if (!g_fs.initialized || !old_path || !new_path) {
        return -1;
    }
    
    fs_file_entry_t* file = fs_find_file(old_path);
    if (!file) {
        return -1;
    }
    
    // Check if new name already exists
    if (fs_find_file(new_path)) {
        return -1;
    }
    
    strncpy(file->name, new_path, FS_MAX_FILENAME - 1);
    file->name[FS_MAX_FILENAME - 1] = '\0';
    
    return 0;
}

// Remove directory
int fs_rmdir(const char* path) {
    return fs_delete(path);
}

// Truncate file
int fs_truncate(const char* path, size_t size) {
    fs_file_entry_t* file = fs_find_file(path);
    if (!file) {
        return -1;
    }
    
    if (size < file->size) {
        file->size = size;
    }
    
    return 0;
}

// Copy file
int fs_copy(const char* src, const char* dest) {
    if (!g_fs.initialized || !src || !dest) {
        return -1;
    }
    
    // Open source
    fs_fd_t src_fd = fs_open(src, FS_O_RDONLY);
    if (src_fd < 0) {
        return -1;
    }
    
    // Create destination
    fs_fd_t dest_fd = fs_open(dest, FS_O_WRONLY | FS_O_CREATE | FS_O_TRUNC);
    if (dest_fd < 0) {
        fs_close(src_fd);
        return -1;
    }
    
    // Copy data
    char buffer[512];
    int bytes_read;
    while ((bytes_read = fs_read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (fs_write(dest_fd, buffer, bytes_read) != bytes_read) {
            fs_close(src_fd);
            fs_close(dest_fd);
            return -1;
        }
    }
    
    fs_close(src_fd);
    fs_close(dest_fd);
    
    return 0;
}
