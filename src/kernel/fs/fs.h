/**
 * CompileOS Simple Filesystem - Header
 * 
 * In-memory filesystem with basic file operations
 */

#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define FS_MAX_FILES 64
#define FS_MAX_FILENAME 64
#define FS_MAX_FILE_SIZE (64 * 1024) // 64KB per file

// File types
typedef enum {
    FS_TYPE_FILE = 0,
    FS_TYPE_DIRECTORY
} fs_file_type_t;

// File permissions
typedef enum {
    FS_PERM_READ = 0x01,
    FS_PERM_WRITE = 0x02,
    FS_PERM_EXECUTE = 0x04
} fs_permissions_t;

// File descriptor
typedef int fs_fd_t;

// File information
typedef struct {
    char name[FS_MAX_FILENAME];
    fs_file_type_t type;
    size_t size;
    uint8_t permissions;
    uint32_t created_time;
    uint32_t modified_time;
} fs_file_info_t;

// Filesystem initialization
int fs_init(void);
void fs_shutdown(void);
bool fs_is_initialized(void);

// File operations
fs_fd_t fs_open(const char* path, int flags);
int fs_close(fs_fd_t fd);
int fs_read(fs_fd_t fd, void* buffer, size_t size);
int fs_write(fs_fd_t fd, const void* buffer, size_t size);
int fs_seek(fs_fd_t fd, int offset, int whence);
int fs_tell(fs_fd_t fd);

// File management
int fs_create(const char* path);
int fs_delete(const char* path);
int fs_exists(const char* path);
int fs_rename(const char* old_path, const char* new_path);
int fs_get_info(const char* path, fs_file_info_t* info);

// Directory operations
int fs_mkdir(const char* path);
int fs_rmdir(const char* path);
int fs_list(const char* path, fs_file_info_t* files, size_t max_files, size_t* count);

// Utility functions
int fs_get_size(const char* path);
int fs_truncate(const char* path, size_t size);
int fs_copy(const char* src, const char* dest);

// File flags
#define FS_O_RDONLY 0x01
#define FS_O_WRONLY 0x02
#define FS_O_RDWR   0x03
#define FS_O_CREATE 0x04
#define FS_O_TRUNC  0x08
#define FS_O_APPEND 0x10

// Seek whence
#define FS_SEEK_SET 0
#define FS_SEEK_CUR 1
#define FS_SEEK_END 2

#endif // FS_H
