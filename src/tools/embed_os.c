/**
 * CompileOS OS Embedding Tool
 * 
 * Embeds the entire OS into a splash screen BMP file
 * Creates a self-contained bootable image
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// BMP header structure
typedef struct {
    uint16_t signature;      // "BM"
    uint32_t file_size;      // File size in bytes
    uint32_t reserved;       // Reserved (must be 0)
    uint32_t data_offset;    // Offset to image data
} __attribute__((packed)) bmp_header_t;

// BMP info header structure
typedef struct {
    uint32_t header_size;    // Size of this header
    int32_t width;           // Image width
    int32_t height;          // Image height
    uint16_t planes;         // Number of color planes
    uint16_t bits_per_pixel; // Bits per pixel
    uint32_t compression;    // Compression type
    uint32_t image_size;     // Image data size
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} __attribute__((packed)) bmp_info_header_t;

// OS embedding header
typedef struct {
    uint32_t magic;          // "COS1" - CompileOS v1
    uint32_t os_size;        // Size of embedded OS
    uint32_t os_offset;      // Offset to OS data
    uint32_t checksum;       // OS data checksum
    char version[16];        // OS version string
    char description[64];    // OS description
} __attribute__((packed)) os_embed_header_t;

// Function prototypes
int embed_os_into_bmp(const char* bmp_file, const char* os_file, const char* output_file);
int extract_os_from_bmp(const char* embedded_file, const char* output_file);
int verify_embedded_os(const char* embedded_file);
uint32_t calculate_checksum(const uint8_t* data, size_t size);

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: %s <command> <input> <output>\n", argv[0]);
        printf("Commands:\n");
        printf("  embed <bmp_file> <os_file> <output_file>  - Embed OS into BMP\n");
        printf("  extract <embedded_file> <output_file>     - Extract OS from embedded file\n");
        printf("  verify <embedded_file>                    - Verify embedded OS\n");
        return 1;
    }
    
    const char* command = argv[1];
    
    if (strcmp(command, "embed") == 0) {
        if (argc != 5) {
            printf("Usage: %s embed <bmp_file> <os_file> <output_file>\n", argv[0]);
            return 1;
        }
        return embed_os_into_bmp(argv[2], argv[3], argv[4]);
    }
    else if (strcmp(command, "extract") == 0) {
        if (argc != 4) {
            printf("Usage: %s extract <embedded_file> <output_file>\n", argv[0]);
            return 1;
        }
        return extract_os_from_bmp(argv[2], argv[3]);
    }
    else if (strcmp(command, "verify") == 0) {
        if (argc != 3) {
            printf("Usage: %s verify <embedded_file>\n", argv[0]);
            return 1;
        }
        return verify_embedded_os(argv[2]);
    }
    else {
        printf("Unknown command: %s\n", command);
        return 1;
    }
}

/**
 * Embed OS into BMP file
 */
int embed_os_into_bmp(const char* bmp_file, const char* os_file, const char* output_file) {
    FILE* bmp_fp = fopen(bmp_file, "rb");
    if (!bmp_fp) {
        printf("Error: Cannot open BMP file %s\n", bmp_file);
        return 1;
    }
    
    FILE* os_fp = fopen(os_file, "rb");
    if (!os_fp) {
        printf("Error: Cannot open OS file %s\n", os_file);
        fclose(bmp_fp);
        return 1;
    }
    
    FILE* out_fp = fopen(output_file, "wb");
    if (!out_fp) {
        printf("Error: Cannot create output file %s\n", output_file);
        fclose(bmp_fp);
        fclose(os_fp);
        return 1;
    }
    
    // Read BMP header
    bmp_header_t bmp_header;
    if (fread(&bmp_header, sizeof(bmp_header), 1, bmp_fp) != 1) {
        printf("Error: Cannot read BMP header\n");
        fclose(bmp_fp);
        fclose(os_fp);
        fclose(out_fp);
        return 1;
    }
    
    // Read BMP info header
    bmp_info_header_t bmp_info;
    if (fread(&bmp_info, sizeof(bmp_info), 1, bmp_fp) != 1) {
        printf("Error: Cannot read BMP info header\n");
        fclose(bmp_fp);
        fclose(os_fp);
        fclose(out_fp);
        return 1;
    }
    
    // Get OS file size
    fseek(os_fp, 0, SEEK_END);
    uint32_t os_size = ftell(os_fp);
    fseek(os_fp, 0, SEEK_SET);
    
    // Read OS data
    uint8_t* os_data = malloc(os_size);
    if (!os_data) {
        printf("Error: Cannot allocate memory for OS data\n");
        fclose(bmp_fp);
        fclose(os_fp);
        fclose(out_fp);
        return 1;
    }
    
    if (fread(os_data, 1, os_size, os_fp) != os_size) {
        printf("Error: Cannot read OS data\n");
        free(os_data);
        fclose(bmp_fp);
        fclose(os_fp);
        fclose(out_fp);
        return 1;
    }
    
    // Calculate checksum
    uint32_t checksum = calculate_checksum(os_data, os_size);
    
    // Create OS embed header
    os_embed_header_t os_header;
    os_header.magic = 0x31534F43; // "COS1"
    os_header.os_size = os_size;
    os_header.os_offset = bmp_header.file_size;
    os_header.checksum = checksum;
    strncpy(os_header.version, "0.1.0", sizeof(os_header.version) - 1);
    strncpy(os_header.description, "CompileOS Embedded in Splash Screen", sizeof(os_header.description) - 1);
    
    // Write new BMP header (with updated file size)
    bmp_header.file_size += os_size + sizeof(os_header);
    fwrite(&bmp_header, sizeof(bmp_header), 1, out_fp);
    
    // Write BMP info header
    fwrite(&bmp_info, sizeof(bmp_info), 1, out_fp);
    
    // Copy BMP data (skip headers)
    fseek(bmp_fp, sizeof(bmp_header) + sizeof(bmp_info), SEEK_SET);
    uint8_t buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), bmp_fp)) > 0) {
        fwrite(buffer, 1, bytes_read, out_fp);
    }
    
    // Write OS embed header
    fwrite(&os_header, sizeof(os_header), 1, out_fp);
    
    // Write OS data
    fwrite(os_data, 1, os_size, out_fp);
    
    // Cleanup
    free(os_data);
    fclose(bmp_fp);
    fclose(os_fp);
    fclose(out_fp);
    
    printf("Successfully embedded OS into %s\n", output_file);
    printf("OS size: %u bytes\n", os_size);
    printf("Total file size: %u bytes\n", bmp_header.file_size);
    
    return 0;
}

/**
 * Extract OS from embedded BMP file
 */
int extract_os_from_bmp(const char* embedded_file, const char* output_file) {
    FILE* in_fp = fopen(embedded_file, "rb");
    if (!in_fp) {
        printf("Error: Cannot open embedded file %s\n", embedded_file);
        return 1;
    }
    
    FILE* out_fp = fopen(output_file, "wb");
    if (!out_fp) {
        printf("Error: Cannot create output file %s\n", output_file);
        fclose(in_fp);
        return 1;
    }
    
    // Read BMP header
    bmp_header_t bmp_header;
    if (fread(&bmp_header, sizeof(bmp_header), 1, in_fp) != 1) {
        printf("Error: Cannot read BMP header\n");
        fclose(in_fp);
        fclose(out_fp);
        return 1;
    }
    
    // Read BMP info header
    bmp_info_header_t bmp_info;
    if (fread(&bmp_info, sizeof(bmp_info), 1, in_fp) != 1) {
        printf("Error: Cannot read BMP info header\n");
        fclose(in_fp);
        fclose(out_fp);
        return 1;
    }
    
    // Seek to OS embed header
    fseek(in_fp, bmp_header.data_offset + bmp_info.image_size, SEEK_SET);
    
    // Read OS embed header
    os_embed_header_t os_header;
    if (fread(&os_header, sizeof(os_header), 1, in_fp) != 1) {
        printf("Error: Cannot read OS embed header\n");
        fclose(in_fp);
        fclose(out_fp);
        return 1;
    }
    
    // Verify magic number
    if (os_header.magic != 0x31534F43) {
        printf("Error: Invalid OS embed header magic\n");
        fclose(in_fp);
        fclose(out_fp);
        return 1;
    }
    
    // Read OS data
    uint8_t* os_data = malloc(os_header.os_size);
    if (!os_data) {
        printf("Error: Cannot allocate memory for OS data\n");
        fclose(in_fp);
        fclose(out_fp);
        return 1;
    }
    
    if (fread(os_data, 1, os_header.os_size, in_fp) != os_header.os_size) {
        printf("Error: Cannot read OS data\n");
        free(os_data);
        fclose(in_fp);
        fclose(out_fp);
        return 1;
    }
    
    // Verify checksum
    uint32_t calculated_checksum = calculate_checksum(os_data, os_header.os_size);
    if (calculated_checksum != os_header.checksum) {
        printf("Warning: OS data checksum mismatch\n");
    }
    
    // Write OS data to output file
    fwrite(os_data, 1, os_header.os_size, out_fp);
    
    // Cleanup
    free(os_data);
    fclose(in_fp);
    fclose(out_fp);
    
    printf("Successfully extracted OS to %s\n", output_file);
    printf("OS size: %u bytes\n", os_header.os_size);
    printf("OS version: %s\n", os_header.version);
    printf("OS description: %s\n", os_header.description);
    
    return 0;
}

/**
 * Verify embedded OS
 */
int verify_embedded_os(const char* embedded_file) {
    FILE* fp = fopen(embedded_file, "rb");
    if (!fp) {
        printf("Error: Cannot open embedded file %s\n", embedded_file);
        return 1;
    }
    
    // Read BMP header
    bmp_header_t bmp_header;
    if (fread(&bmp_header, sizeof(bmp_header), 1, fp) != 1) {
        printf("Error: Cannot read BMP header\n");
        fclose(fp);
        return 1;
    }
    
    // Read BMP info header
    bmp_info_header_t bmp_info;
    if (fread(&bmp_info, sizeof(bmp_info), 1, fp) != 1) {
        printf("Error: Cannot read BMP info header\n");
        fclose(fp);
        return 1;
    }
    
    // Seek to OS embed header
    fseek(fp, bmp_header.data_offset + bmp_info.image_size, SEEK_SET);
    
    // Read OS embed header
    os_embed_header_t os_header;
    if (fread(&os_header, sizeof(os_header), 1, fp) != 1) {
        printf("Error: Cannot read OS embed header\n");
        fclose(fp);
        return 1;
    }
    
    // Verify magic number
    if (os_header.magic != 0x31534F43) {
        printf("Error: Invalid OS embed header magic\n");
        fclose(fp);
        return 1;
    }
    
    printf("Embedded OS Verification:\n");
    printf("  Magic: 0x%08X\n", os_header.magic);
    printf("  OS Size: %u bytes\n", os_header.os_size);
    printf("  OS Offset: %u bytes\n", os_header.os_offset);
    printf("  Checksum: 0x%08X\n", os_header.checksum);
    printf("  Version: %s\n", os_header.version);
    printf("  Description: %s\n", os_header.description);
    
    fclose(fp);
    return 0;
}

/**
 * Calculate checksum
 */
uint32_t calculate_checksum(const uint8_t* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum = (checksum << 1) ^ data[i];
    }
    return checksum;
}










