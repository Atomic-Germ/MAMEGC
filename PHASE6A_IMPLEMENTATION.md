# Phase 6A Implementation: SD Card Detection & Mounting

## Immediate Tasks

### 1. Fix SD Card Mounting in osd_gc.c

**Current code (incorrect):**
```c
/* Initialize FAT filesystem for SD card */
if (!fatInitDefault()) {
    printf("Warning: FAT initialization failed\n");
    /* Not fatal - ROMs can be embedded */
}
```

**Replace with (correct for GameCube):**
```c
/* Initialize FAT filesystem for SD Gecko/SD2SP2 */
bool sd_mounted = false;
printf("Detecting SD cards...\n");

/* Try Slot A (carda:) */
if (fatMountSimple("carda", 0)) {
    printf("SD card mounted: carda:/\n");
    sd_mounted = true;
} 
/* Try Slot B (cardb:) if Slot A failed */
else if (fatMountSimple("cardb", 1)) {
    printf("SD card mounted: cardb:/\n");
    sd_mounted = true;
} 
else {
    printf("No SD card detected in either slot\n");
    printf("ROMs must be embedded or loaded via other means\n");
}

return sd_mounted ? 0 : -1; /* Return error if no SD card */
```

### 2. Add SD Card Status Functions

**Add to osd_gc.h:**
```c
/* SD Card functions */
bool osd_sd_available(void);
const char* osd_get_sd_mount(void);
bool osd_test_sd_access(void);
```

**Add to osd_gc.c:**
```c
static char sd_mount_point[16] = "";
static bool sd_is_available = false;

bool osd_sd_available(void) {
    return sd_is_available;
}

const char* osd_get_sd_mount(void) {
    return sd_is_available ? sd_mount_point : NULL;
}

bool osd_test_sd_access(void) {
    if (!sd_is_available) return false;
    
    char test_path[64];
    snprintf(test_path, sizeof(test_path), "%s/", sd_mount_point);
    
    DIR* dir = opendir(test_path);
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}
```

### 3. Update osd_init() Function

**Complete updated osd_init():**
```c
int osd_init(void) {
    /* Initialize timing */
    start_ticks = gettime();
    
    /* Initialize FAT filesystem for SD Gecko/SD2SP2 */
    sd_is_available = false;
    printf("Detecting SD cards...\n");

    /* Try Slot A (carda:) */
    if (fatMountSimple("carda", 0)) {
        strcpy(sd_mount_point, "carda:");
        sd_is_available = true;
        printf("SD card mounted: %s/\n", sd_mount_point);
    } 
    /* Try Slot B (cardb:) if Slot A failed */
    else if (fatMountSimple("cardb", 1)) {
        strcpy(sd_mount_point, "cardb:");
        sd_is_available = true;
        printf("SD card mounted: %s/\n", sd_mount_point);
    } 
    else {
        printf("No SD card detected in either slot\n");
        printf("Check SD Gecko/SD2SP2 connection\n");
    }
    
    /* Test SD card access if mounted */
    if (sd_is_available) {
        if (osd_test_sd_access()) {
            printf("SD card access test: PASSED\n");
        } else {
            printf("SD card access test: FAILED\n");
            sd_is_available = false;
        }
    }
    
    return 0; /* Don't fail if no SD card - allow embedded ROMs */
}
```

### 4. Update osd_exit() Function

**Fix unmount call:**
```c
void osd_exit(void) {
    osd_close_display();
    
    if (sd_is_available) {
        printf("Unmounting SD card...\n");
        fatUnmount(sd_mount_point);
    }
}
```

### 5. Add Include for Directory Operations

**Add to osd_gc.c includes:**
```c
#include <dirent.h>
#include <sys/stat.h>
```

## Testing Steps

### Build Test:
```bash
./docker-build.sh build gc
```

### Hardware Test:
1. Copy `mamegc-gc.dol` to SD card
2. Insert SD card in GameCube SD Gecko (Slot A or B)
3. Boot with Swiss
4. Run DOL file
5. Check console output for SD card detection messages

### Expected Output:
```
Detecting SD cards...
SD card mounted: carda:/
SD card access test: PASSED
```

OR:
```
Detecting SD cards...
SD card mounted: cardb:/
SD card access test: PASSED
```

OR:
```
Detecting SD cards...
No SD card detected in either slot
Check SD Gecko/SD2SP2 connection
```

## Success Criteria for Phase 6A:

✅ Code compiles without errors  
✅ SD card detected in correct slot  
✅ Mount point correctly identified (carda: or cardb:)  
✅ File system access test passes  
✅ Graceful handling when no SD card present  
✅ Existing functionality still works (input, video, etc.)  

## Next Phase:
Once Phase 6A is complete and tested, proceed to Phase 6B (ROM File Detection).

## Troubleshooting:

**If SD card not detected:**
- Verify SD Gecko/SD2SP2 hardware connection
- Check SD card is FAT32 formatted
- Try different SD card
- Verify Swiss can access the same SD card

**If access test fails:**
- SD card may be corrupted
- Wrong filesystem format (use FAT32)
- Hardware connection issue

**If build fails:**
- Check that libfat is properly linked in Makefile.gc
- Verify all includes are present
- Check for syntax errors in code additions