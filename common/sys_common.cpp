#include "sys_common.h"
#include <io.h>
#include <direct.h>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

void DisplayErrorBox(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and clean up

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40)*sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

int list_files(std::list<std::string> &files, const char *dir)
{
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.

    StringCchLength(dir, MAX_PATH, &length_of_arg);

    if (length_of_arg > (MAX_PATH - 3))
    {
        _tprintf(TEXT("\nDirectory path is too long.\n"));
        return (-1);
    }

    _tprintf(TEXT("\nTarget directory is %s\n\n"), dir);

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.

    StringCchCopy(szDir, MAX_PATH, dir);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind)
    {
        DisplayErrorBox(TEXT("FindFirstFile"));
        return dwError;
    }

    // List all the files in the directory with some info about them.

    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            _tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
        }
        else
        {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            _tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
            files.push_back(ffd.cFileName);
        }
    } 
    while (FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        DisplayErrorBox(TEXT("FindFirstFile"));
    }

    FindClose(hFind);
    return dwError;
}

std::string extract_filepath(const std::string& s)
{
    std::string::size_type pos = 0;
    if ((pos = s.find_last_of(":\\")) != std::string::npos)
    {
        return s.substr(0, pos + 1);
    }
    return s;
}

std::string extract_filename(const std::string& s)
{
    std::string::size_type pos = 0;
    if ((pos = s.find_last_of(":\\")) != std::string::npos)
    {
        return s.substr(pos + 1, s.length() - (pos + 1));
    }
    return s;
}

std::string remove_filename_extension(const std::string& s)
{
    std::string::size_type pos = 0;
    if ((pos = s.find_last_of(".")) != std::string::npos)
    {
        return s.substr(0, pos);
    }
    return s;
}

std::string extract_filename_extension(const std::string& s)
{
    std::string::size_type pos = 0;
    if ((pos = s.find_last_of(".")) != std::string::npos)
    {
        return s.substr(pos, s.length() - pos);
    }
    else
    {
        return "";
    }
}

int create_dir_safely(const char *dir)
{
    char tmp[MAX_PATH];
    int off;

    for (off = 0; off < (sizeof(tmp) - 1) && dir[off] != '\0'; off++)
    {
        if (dir[off] == '\\' || dir[off] == '/')
        {
            tmp[off] = '\\';
            tmp[off + 1] = '\0';

            if (-1 == _access(tmp, 0))
            {
                if (-1 == _mkdir(tmp))
                {
                    return -1;
                }
            }
        }
        else
        {
            tmp[off] = dir[off];
        }
    }

    tmp[off] = '\0';

    if (off > 0 && -1 == _access(tmp, 0))
    {
        if (-1 == _mkdir(tmp))
        {
            return -1;
        }
    }

    return off;
}
