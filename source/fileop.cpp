#include "fileop.h"

#include <gccore.h>
#include <fat.h>
#include <sdcard/gcsd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

// Device map (GameCube only)
static const DISC_INTERFACE* s_io_carda = &__io_gcsda;
static const DISC_INTERFACE* s_io_cardb = &__io_gcsdb;
static const DISC_INTERFACE* s_io_port2 = &__io_gcsd2;

struct device_map {
    int id;
    const char* name;   // e.g., "carda"
    const char* mount;  // e.g., "carda:"
    const DISC_INTERFACE** disc;
};

static device_map s_devices[] = {
    { DEVICE_SD_SLOTA, "carda", "carda:", &s_io_carda },
    { DEVICE_SD_SLOTB, "cardb", "cardb:", &s_io_cardb },
    { DEVICE_SD_PORT2, "port2", "port2:", &s_io_port2 },
};

static bool s_mounted[DEVICE_COUNT] = {false,false,false};
static char g_app_base[256] = {0};

static const device_map* dev_by_id(int id) {
    for (unsigned i=0;i<sizeof(s_devices)/sizeof(s_devices[0]);++i)
        if (s_devices[i].id == id) return &s_devices[i];
    return NULL;
}

static const device_map* dev_from_path(const char* path, int* out_id) {
    if (!path) return NULL;
    for (unsigned i=0;i<sizeof(s_devices)/sizeof(s_devices[0]);++i) {
        const device_map* d = &s_devices[i];
        size_t len = strlen(d->mount);
        if (strncmp(path, d->mount, len) == 0) {
            if (out_id) *out_id = d->id;
            return d;
        }
    }
    return NULL;
}

bool fs_mount_all_fat(void) {
    bool any = false;
    for (int i=0;i<DEVICE_COUNT;i++) {
        const device_map* d = dev_by_id(i);
        if (!d) continue;
        if (s_mounted[i]) { any = true; continue; }
        DISC_INTERFACE* disc = *d->disc;
        if (disc->startup(disc) && fatMountSimple(d->name, disc)) {
            s_mounted[i] = true;
            any = true;
        }
    }
    return any;
}

void fs_unmount_all_fat(void) {
    for (int i=0;i<DEVICE_COUNT;i++) {
        const device_map* d = dev_by_id(i);
        if (!d) continue;
        if (s_mounted[i]) {
            fatUnmount(d->mount);
            s_mounted[i] = false;
        }
    }
}

bool fs_is_mounted(int device) {
    if (device < 0 || device >= DEVICE_COUNT) return false;
    return s_mounted[device];
}

bool fs_change_interface_device(int device) {
    const device_map* d = dev_by_id(device);
    if (!d) return false;
    if (s_mounted[device]) return true;
    DISC_INTERFACE* disc = *d->disc;
    if (disc->startup(disc) && fatMountSimple(d->name, disc)) {
        s_mounted[device] = true;
        return true;
    }
    return false;
}

bool fs_find_device_from_path(const char* filepath, int* out_device) {
    return dev_from_path(filepath, out_device) != NULL;
}

bool fs_change_interface_path(const char* filepath) {
    int dev = -1;
    if (!fs_find_device_from_path(filepath, &dev)) return false;
    return fs_change_interface_device(dev);
}

long fs_file_size(const char* filepath) {
    if (!filepath) return -1;
    if (!fs_change_interface_path(filepath)) return -1;
    FILE* f = fopen(filepath, "rb");
    if (!f) return -1;
    fseeko(f, 0, SEEK_END);
    long sz = (long)ftello(f);
    fclose(f);
    return sz;
}

long fs_read_file(const char* filepath, void* buf, size_t bufsize) {
    if (!filepath || !buf || bufsize == 0) return -1;
    if (!fs_change_interface_path(filepath)) return -1;
    FILE* f = fopen(filepath, "rb");
    if (!f) return -1;
    size_t total = 0;
    while (total < bufsize) {
        size_t n = fread((char*)buf + total, 1, bufsize - total, f);
        if (n == 0) break;
        total += n;
    }
    fclose(f);
    return (long)total;
}

long fs_write_file(const char* filepath, const void* buf, size_t size) {
    if (!filepath || !buf || size == 0) return -1;
    if (!fs_change_interface_path(filepath)) return -1;
    FILE* f = fopen(filepath, "wb");
    if (!f) return -1;
    size_t total = 0;
    while (total < size) {
        size_t n = fwrite((const char*)buf + total, 1, size - total, f);
        if (n == 0) break;
        total += n;
    }
    fclose(f);
    return (long)total;
}

bool fs_exists(const char* path) {
    if (!path) return false;
    struct stat st;
    if (!fs_change_interface_path(path)) return false;
    return stat(path, &st) == 0;
}

bool fs_mkdir(const char* path) {
    if (!path) return false;
    // Ensure device mounted for this path
    if (!fs_change_interface_path(path)) return false;
    DIR* d = opendir(path);
    if (d) { closedir(d); return true; }
    return mkdir(path, 0777) == 0;
}

bool fs_mkdir_p(const char* path) {
    if (!path) return false;
    if (!fs_change_interface_path(path)) return false;

    char tmp[256];
    size_t len = strlen(path);
    if (len >= sizeof(tmp)) return false;
    strcpy(tmp, path);

    // Normalize: ensure after mount prefix we iterate components
    // e.g., "carda:/mamegc/dir"
    // Skip device prefix like "carda:" (5 or 6 chars) and optional '/'
    const char* p = strchr(tmp, ':');
    size_t start = 0;
    if (p) {
        start = (size_t)(p - tmp + 1); // points to ':' index + 1
        if (tmp[start] != '/') {
            // insert a slash after ':' if missing
            if (len + 1 >= sizeof(tmp)) return false;
            memmove(tmp + start + 1, tmp + start, len - start + 1);
            tmp[start] = '/';
            len++; // updated length
        }
        start++; // start after '/'
    }

    for (size_t i = start; i <= len; ++i) {
        if (tmp[i] == '/' || tmp[i] == '\\' || tmp[i] == '\0') {
            char saved = tmp[i];
            tmp[i] = '\0';
            if (!fs_mkdir(tmp)) {
                // if not exists, mkdir will try; if fails for other reason, still continue only if already exists
                // fs_mkdir returns true if exists or created
                tmp[i] = saved;
            } else {
                tmp[i] = saved;
            }
        }
    }
    return true;
}

int fs_list_dir(const char* path, fs_dir_entry* out_entries, int max_entries) {
    if (!path || !out_entries || max_entries <= 0) return -1;
    if (!fs_change_interface_path(path)) return -1;
    DIR* d = opendir(path);
    if (!d) return -1;

    int count = 0;
    struct dirent* ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0) continue;
        if (count >= max_entries) break;

        fs_dir_entry* e = &out_entries[count];
        strncpy(e->name, ent->d_name, sizeof(e->name)-1);
        e->name[sizeof(e->name)-1] = '\0';

        // Determine directory via stat on full path
        char full[512];
        fs_path_join(full, sizeof(full), path, ent->d_name);
        struct stat st;
        if (stat(full, &st) == 0 && S_ISDIR(st.st_mode))
            e->is_dir = true;
        else
            e->is_dir = false;

        count++;
    }
    closedir(d);
    return count;
}

int fs_path_join(char* out, size_t out_size, const char* base, const char* subpath) {
    if (!out || !base || !subpath || out_size == 0) return -1;
    size_t bl = strlen(base);
    size_t sl = strlen(subpath);

    bool base_has_sep = false;
    if (bl > 0) {
        char last = base[bl-1];
        base_has_sep = (last == '/' || last == ':');
    }

    // Skip leading slash in subpath
    const char* sp = subpath;
    while (*sp == '/') sp++;

    int written = 0;
    if (base_has_sep && base[bl-1] == ':') {
        // Need to add '/'
        written = snprintf(out, out_size, "%s/%s", base, sp);
    } else if (base_has_sep) {
        written = snprintf(out, out_size, "%s%s", base, sp);
    } else {
        written = snprintf(out, out_size, "%s/%s", base, sp);
    }
    if (written < 0 || (size_t)written >= out_size) return -1;
    return written;
}

const char* fs_app_base_path(void) {
    if (g_app_base[0]) return g_app_base;

    // Preferred order: SD2SP2 (port2), then Slot A, then Slot B
    const int order[] = { DEVICE_SD_PORT2, DEVICE_SD_SLOTA, DEVICE_SD_SLOTB };
    for (unsigned i=0;i<sizeof(order)/sizeof(order[0]);++i) {
        int dev = order[i];
        if (!fs_change_interface_device(dev)) continue;
        const device_map* d = dev_by_id(dev);
        if (!d) continue;
        char base[256];
        snprintf(base, sizeof(base), "%s/mamegc", d->mount);
        if (fs_mkdir_p(base)) {
            strncpy(g_app_base, base, sizeof(g_app_base)-1);
            g_app_base[sizeof(g_app_base)-1] = '\0';
            return g_app_base;
        }
    }
    return NULL;
}
