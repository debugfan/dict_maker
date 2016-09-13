#include "csv2ycd.h"

#include <string>
#include <fstream>
#include <stdio.h>
#include <Windows.h>
#include "../common/common.h"
#include "../common/csv_common.h"
using namespace std;

string escape_to_ycd(string src)
{
	string dest = src;
	replace_all(dest, "\\\"", "\"");
	replace_all(dest, "\\r\\n", "<br>");
	return dest;
}

void output_ycd_item(ofstream &out, string word_name, string word_content)
{
	out << "yunci4_item_name: " << word_name << endl;
	out << "yunci4_item_content: " << escape_to_ycd(word_content) << endl;
	out << endl;
}

BOOL list2ycd(const list<word_item_t> &word_list, const char *filename)
{
	std::list<word_item_t>::const_iterator iter;
	ofstream ofs(filename, ios::out);

    if (!ofs.is_open())
    {
        printf("Failed to open file.\n");
    }

	for(iter = word_list.begin(); iter != word_list.end(); iter++)
	{
		output_ycd_item(ofs, iter->name, iter->desc);
	}

	return TRUE;
}

BOOL csv2ycd(const char *in_fname, const char *out_fname)
{
	list<word_item_t> word_list;
	csv2list(in_fname, word_list);
	list2ycd(word_list, out_fname);
	return TRUE;
}
