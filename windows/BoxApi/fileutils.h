/* fileutils.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef FILETYPES_H
#define FYLETYPES_H

// File helpers

enum FileType
{
    FILE_UNDEFIED = 0,
    FILE_IMAGE,
    FILE_VIDEO,
    FILE_AI,
    FILE_PSD,
    FILE_BOOKMARK,
    FILE_DOC,
    FILE_XLS,
    FILE_PPT,
    FILE_FLA,
    FILE_MUSIC,
    FILE_HTML,
    FILE_ZIP,

    FILE_MAX
};

// Try to guess file type base on it's name

FileType getFileType(const QString& fileName);
bool renameFile(const QString& source, const QString& destination);
void deletePath(const QString& path);
void removeUntouched(const QString& path);
void removeUntouched(const QString path, QDateTime border);                           

#endif // FYLETYPES_H
