#ifndef SYS_COMMON_H
#define SYS_COMMON_H

#include <string>
#include <list>

int list_files(std::list<std::string> &files, const char *dir);
std::string extract_filepath(const std::string& s);
std::string extract_filename(const std::string& s);
std::string remove_filename_extension(const std::string& s);
std::string extract_filename_extension(const std::string& s);
int create_dir_safely(const char *dir);

#endif
