#ifndef DICT_CONVERTER_COMMON_H
#define DICT_CONVERTER_COMMON_H

#include <string>
#include <stdio.h>
#include <fstream>
#include <windows.h>

void replace_all(std::string& str, const std::string& from, const std::string& to);

BOOL is_blank_in_line(const char c);
const char *skip_blank(const char *p);
BOOL is_crlf(const char c);

BOOL copy_filetype_if(std::ifstream &ifs, std::ofstream &ofs);
BOOL skip_filetype_if(std::ifstream &ifs);

BOOL is_english_word_char(char c);
BOOL is_word_en(const char *w);

const char *read_word(char *w, int *len, const char *p);

#endif