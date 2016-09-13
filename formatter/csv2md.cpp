#include "csv2md.h"

#include <string>
#include <fstream>
#include <stdio.h>
#include <Windows.h>
#include "../common/common.h"
#include "../common/csv_common.h"
using namespace std;

string escape_to_md(string src)
{
	string dest = src;
	replace_all(dest, "\\\"", "\"");
	replace_all(dest, "\\r\\n", "<br>");
	return dest;
}

void output_md_item(ofstream &out, string word_name, string word_content)
{
	out << word_name << endl;
	out << escape_to_md(word_content) << endl;
	out << "</>" << endl;
}

BOOL list2md(const list<word_item_t> &word_list, const char *filename)
{
	std::list<word_item_t>::const_iterator iter;
	ofstream ofs(filename, ios::out);

    if (!ofs.is_open())
    {
        printf("Failed to open file.\n");
    }

	for(iter = word_list.begin(); iter != word_list.end(); iter++)
	{
		output_md_item(ofs, iter->name, iter->desc);
	}

	return TRUE;
}

BOOL csv2md(const char *in_fname, const char *out_fname)
{
	list<word_item_t> word_list;
	csv2list(in_fname, word_list);
	list2md(word_list, out_fname);
	return TRUE;
}
