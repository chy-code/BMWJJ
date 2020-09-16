#ifndef _FILE_SYS_UTIL_H
#define _FILE_SYS_UTIL_H

#include <stdio.h>
#include "rl_fs.h"


fsStatus FileSys_OpenFile(const char *filename,
                          const char *mode,
                          FILE **stream);

fsStatus FileSys_WriteBinary(const char *filename,
                             const void *data,
                             size_t bytesToWrite);

fsStatus FileSys_ReadBinary(const char *filename,
                            void *buf,
                            size_t bytesToRead);

fsStatus FileSys_AppendBinary(const char *filename,
                              void *buf,
                              size_t bytesToWrite);

fsStatus FileSys_Find(const char * pattern, fsFileInfo *fileInfo);

fsStatus FileSys_FindFile(const char *filename);

fsStatus FileSys_DeleteDirectory(const char *path);

fsStatus FileSys_DeleteFile(const char *filename);

fsStatus FileSys_GetDriveInfo(int *capacity, int *free);

const char * FileSys_StrError(fsStatus status);

#endif
