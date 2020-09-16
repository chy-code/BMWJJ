
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "cmsis_os2.h"
#include "FileSysUtil.h"
#include "DbgPrint.h"


static bool _inited = false;
static bool _mounted = false;


fsStatus FileSys_SetReady(void)
{
    fsStatus status;
    if (!_inited)
    {
        status = finit(""); // Use current drive
        if (status != fsOK)
        {
            DbgPrint("finit failed: %s\n", FileSys_StrError(status));
            return status;
        }

        _inited = true;
    }

    if (!_mounted)
    {
        status = fmount(""); // Use current drive
        if (status != fsOK)
        {
            DbgPrint("fmount failed: %s\n", FileSys_StrError(status));
            return status;
        }

        _mounted = true;
    }

    return fsOK;
}



fsStatus
FileSys_OpenFile(const char *filename,
                 const char *mode,
                 FILE **stream)
{
    fsStatus status = FileSys_SetReady();
    if (status != fsOK)
        return status;

    *stream = fopen(filename, mode);
    if (*stream == NULL)
        return fsFileNotFound;

    return fsOK;
}


fsStatus FileSys_WriteFile(const char *filename,
                           const char *format,
                           ...)
{
    FILE *stream;
    int ret;

    fsStatus status = FileSys_OpenFile(filename, "w", &stream);
    if (status != fsOK)
        return status;

    va_list args;
    va_start(args, format);
    ret = vfprintf(stream, format, args);
    va_end(args);

    fclose(stream);

    if (ret <= 0)
        return fsError;

    return fsOK;
}



fsStatus FileSys_ReadFile(const char *filename,
                          const char *format,
                          ...)
{
    FILE *stream;
    int ret;

    fsStatus status = FileSys_OpenFile(filename, "r", &stream);
    if (status != fsOK)
        return status;

    va_list args;
    va_start(args, format);
    ret = vfscanf(stream, format, args);
    va_end(args);

    fclose(stream);

    if (ret <= 0)
        return fsError;

    return fsOK;
}


fsStatus
FileSys_WriteBinary(const char *filename,
                    const void *data,
                    size_t bytesToWrite)
{
    FILE *stream;
    fsStatus status = FileSys_OpenFile(filename, "wb", &stream);
    if (status != fsOK)
        return status;

    size_t nb = fwrite(data, bytesToWrite, 1, stream);
    fclose(stream);

    if (nb != 1)
        return fsDriverError;

    return fsOK;
}



fsStatus
FileSys_ReadBinary(const char *filename,
                   void *buf,
                   size_t bytesToRead)
{
    FILE *stream;
    fsStatus status = FileSys_OpenFile(filename, "rb", &stream);
    if (status != fsOK)
        return status;

    size_t nb = fread(buf, bytesToRead, 1, stream);
    fclose(stream);

    if (nb != 1)
        return fsDriverError;

    return fsOK;
}


fsStatus FileSys_AppendBinary(const char *filename,
                              void *buf,
                              size_t bytesToWrite)
{
    FILE *stream;
    fsStatus status = FileSys_OpenFile(filename, "ab", &stream);
    if (status != fsOK)
        return status;

    size_t nb = fwrite(buf, bytesToWrite, 1, stream);
    fclose(stream);

    if (nb != 1)
        return fsDriverError;

    return fsOK;
}


fsStatus FileSys_Find(const char * pattern, fsFileInfo *fileInfo)
{
    fsStatus status = FileSys_SetReady();
    if (status != fsOK)
        return status;

    return ffind(pattern, fileInfo);
}


fsStatus
FileSys_FindFile(const char *filename)
{
    FILE *stream = NULL;
    fsStatus status = FileSys_OpenFile(filename, "r", &stream);
    if (status == fsOK)
        fclose(stream);
    return status;
}


fsStatus
FileSys_DeleteFile(const char *filename)
{
    fsStatus status = FileSys_SetReady();
    if (status != fsOK)
        return status;

    status = fdelete(filename, NULL);
    if (status != fsOK)
    {
        if (status != fsFileNotFound)
            return status;
    }

    return fsOK;
}


fsStatus
FileSys_DeleteDirectory(const char *path)
{
    fsStatus status = FileSys_SetReady();
    if (status != fsOK)
        return status;

    return frmdir(path, "/S");
}



fsStatus FileSys_GetDriveInfo(int *capacity, int *free)
{
    fsDriveInfo driveInfo;
    fsStatus status = finfo("", &driveInfo);
    if (status != fsOK)
        return status;

    *capacity = driveInfo.capacity  / 1000000; // bytes to MB
    *free = ffree("") / 1000000;

    return fsOK;
}


const char * FileSys_StrError(fsStatus status)
{
    switch (status)
    {
    case fsOK:
        return "FileSystem: Operation succeeded.";
    case fsError:
        return "FileSystem: Unspecified error.";
    case fsUnsupported:
        return "FileSystem: Operation not supported.";
    case fsAccessDenied:
        return "FileSystem: Resource access denied.";
    case fsInvalidParameter:
        return "FileSystem: Invalid parameter specified.";
    case fsInvalidDrive:
        return "FileSystem: Nonexistent drive.";
    case fsInvalidPath:
        return "FileSystem: Invalid path specified.";
    case fsUninitializedDrive:
        return "FileSystem: Drive is uninitialized.";
    case fsDriverError:
        return "FileSystem: Read/write error.";
    case fsMediaError:
        return "FileSystem: Media error.";
    case fsNoMedia:
        return "FileSystem: No media, or not initialized.";
    case fsNoFileSystem:
        return "FileSystem: File system is not formatted.";
    case fsNoFreeSpace:
        return "FileSystem: No free space available.";
    case fsFileNotFound:
        return "FileSystem: Requested file not found.";
    case fsDirNotEmpty:
        return "FileSystem: The directory is not empty.";
    case fsTooManyOpenFiles:
        return "FileSystem: Too many open files.";
    default:
        return "FileSystem: Unknown error.";
    }
}

