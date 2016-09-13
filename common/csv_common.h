#ifndef CSV_COMMON_H
#define CSV_COMMON_H

#include <string>
#include <list>
using std::string;
using std::list;
#include <windows.h>

#define CSV_QUOTE	"\\\""
#define CSV_CRLF	"\\r\\n"

std::string merge_csv_crlf(std::string &src, int count);
std::string escape_to_csv(std::string src);

typedef struct  
{
	std::string name;
	std::string desc;
} word_item_t;

BOOL csv2list(const char *filename, list<word_item_t>& word_list);
BOOL list2csv(const list<word_item_t> &word_list, const char *filename);
BOOL merge_csv(const char *in_fname1, const char *in_fname2, const char *out_fname, const char *sepr);

#endif