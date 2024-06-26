#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

// thank you https://stackoverflow.com/a/3208376
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


size_t fileLen(FILE *file) // will set file back to the start so be carefull if calling from middle of file <3
{
    fseek(file, 0L, SEEK_END);
    size_t r = ftell(file);
    rewind(file);

    return r;
}

bool testDiskWriteDataAndCheck(FILE *disk, size_t diskLen, uint8_t data)
{
    bool ret = true;

    for (size_t i = 0; i < diskLen; i++)
    {
        fputc(data, disk);
    }

    rewind(disk);

    for (size_t i = 0; i < diskLen; i++)
    {
        uint8_t dat = fgetc(disk);

        if (dat != data)
        {
            printf("expected " BYTE_TO_BINARY_PATTERN " got " BYTE_TO_BINARY_PATTERN " at 0x%zX\n", BYTE_TO_BINARY(data), BYTE_TO_BINARY(dat), i);

            ret = false;
        }
    }

    rewind(disk);

    return ret;
}

bool testDisk(char *diskPath)
{
    bool ret = false;
    FILE *disk;
    size_t diskLen;

    disk = fopen(diskPath, "r+");
    diskLen = fileLen(disk);

    printf("\ndisk: %s\nsize: %zu\n\n", diskPath, diskLen);

    // write 1's then test
    printf("all 1's test\n");
    ret = (!testDiskWriteDataAndCheck(disk, diskLen, 0b11111111)) || ret;

    // write 0's then test
    printf("completed\nall 0's test\n");
    ret = (!testDiskWriteDataAndCheck(disk, diskLen, 0b00000000)) || ret;

    // write 10's then test
    printf("completed\nalternated 1's and 0's test\n");
    ret = (!testDiskWriteDataAndCheck(disk, diskLen, 0b10101010)) || ret;

    printf("completed\n");

    return !ret;

}

int main(int argc, char **argv)
{
    printf("disk test - a primative disk testing tool by rose apollo\nDO NOT USE unless you have read the src code and KNOW what you are doing\nthis tool will ONLY run on POSIX compliant systems - eg: freeBSD / macOS / Linux\n");

    char *diskPath;
    bool allocDiskPath = false;

    if (argc >= 2)
        diskPath = argv[1];
    else
    {
        diskPath = malloc(sizeof(char) * 4096);
        printf("path to disk: ");
        scanf("%4096s", diskPath);
        printf("\n");
        allocDiskPath = true;
    }

    bool succ = testDisk(diskPath);

    if (succ)
        printf("\nPASSED!\n");
    else
        printf("\nFAILED!\n");

    if (allocDiskPath)
        free(diskPath);
    
    return succ - 1;
}