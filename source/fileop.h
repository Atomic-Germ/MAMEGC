#pragma once
#include <stddef.h>
#include <stdbool.h>

// Simple filesystem layer for GameCube (SD Gecko in Slot A/B/Port2)
// Conventions inspired by snes9xGC, but trimmed and self-contained.

#ifdef __cplusplus
extern "C" {
#endif

enum gc_device {
    DEVICE_SD_SLOTA = 0,
    DEVICE_SD_SLOTB = 1,
    DEVICE_SD_PORT2 = 2,
    DEVICE_COUNT
};

// Mount/unmount
bool fs_mount_all_fat(void);   // Try to mount carda:/, cardb:/, port2:/ (best-effort)
void fs_unmount_all_fat(void);

// Device helpers
bool fs_change_interface_device(int device);
bool fs_change_interface_path(const char* filepath);
bool fs_find_device_from_path(const char* filepath, int* out_device);
bool fs_is_mounted(int device);

// File IO
long fs_file_size(const char* filepath);                 // returns size or -1
long fs_read_file(const char* filepath, void* buf, size_t bufsize); // returns bytes read or -1
long fs_write_file(const char* filepath, const void* buf, size_t size); // returns bytes written or -1
bool fs_exists(const char* path);

// Directory
bool fs_mkdir(const char* path);
bool fs_mkdir_p(const char* path);

typedef struct {
    char name[256];
    bool is_dir;
} fs_dir_entry;

// Returns number of entries written to out_entries, or -1 on error
int fs_list_dir(const char* path, fs_dir_entry* out_entries, int max_entries);

// App path utilities
const char* fs_app_base_path(void); // e.g., "port2:/mamegc" or "carda:/mamegc"
int fs_path_join(char* out, size_t out_size, const char* base, const char* subpath);

#ifdef __cplusplus
}
#endif
