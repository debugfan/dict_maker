#include "csv_common.h"
#include "common.h"
#include <fstream>
using namespace std;

char separation[50] = {0};

std::string merge_csv_crlf(std::string &src, int count)
{
	string dest = src;
	for(int i = 0; i < count; i++)
	{
		replace_all(dest, CSV_CRLF CSV_CRLF, CSV_CRLF);
	}
	return dest;
}

std::string escape_to_csv(std::string src)
{
	string dest = src;
	replace_all(dest, "\"", CSV_QUOTE);
	replace_all(dest, "\r\n", CSV_CRLF);
	replace_all(dest, "\r", CSV_CRLF);
	replace_all(dest, "\n", CSV_CRLF);
	return merge_csv_crlf(dest, 5);
}

void output_to_csv(ofstream &out, const string &name, const string &desc)
{
	out << name << ",";
	out << "\"" << escape_to_csv(desc) << "\"" << endl;
}

void add_word(list<word_item_t> &word_list, const string &name, const string &desc)
{
	list<word_item_t>::iterator iter;
	for(iter = word_list.begin(); iter != word_list.end(); iter++)
	{
		if(0 == _stricmp(iter->name.c_str(), name.c_str()))
		{
			iter->desc = iter->desc+separation+desc;
			break;
		}
	}

	if(iter == word_list.end())
	{
		word_item_t word_item;
		word_item.name = name;
		word_item.desc = desc;
		word_list.push_back(word_item);
	}
}

BOOL cvs_read_sepr(string &rec)
{
	if(rec.length() == 0)
	{
		return FALSE;
	}

	if(rec[0] == ',')
	{
		rec = rec.substr(1);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL cvs_read_item(string &rec, string &item)
{
	if(rec.length() == 0)
	{
		return FALSE;
	}

	if(rec[0] == '\"')
	{
		int stt;
		unsigned int i;
#define NON_ESCAPE	0
#define IN_ESCAPE	1
		for(i = 1, stt = NON_ESCAPE; i < rec.length(); i++)
		{
			if(stt == IN_ESCAPE)
			{
				item += rec[i];
				stt = NON_ESCAPE;
				continue;
			}
			else if(stt == NON_ESCAPE)
			{
				if(rec[i] == '\"')
				{
					i++;
					break;
				}
				else
				{
					item += rec[i];
					if(rec[i] == '\\')
					{
						stt = IN_ESCAPE;
					}
				}
			}
		}

		rec = rec.substr(i);
	}
	else
	{
		size_t pos = rec.find(',');
		if(pos != string::npos)
		{
			item = rec.substr(0, pos);
			rec = rec.substr(pos);
		}
		else
		{
			item = rec;
			rec = "";
		}
	}

	return TRUE;
}

BOOL parse_csv_line(const string &line, list<word_item_t>& word_list)
{
	string name;
	string content;
	string rec = line;
	if(!cvs_read_item(rec, name))
	{
		fprintf(stderr, "Read word failed");
		return FALSE;
	}

	if(!cvs_read_sepr(rec))
	{
		return FALSE;
	}

	if(!cvs_read_item(rec, content))
	{
		return FALSE;
	}

	add_word(word_list, name, content);
	return TRUE;
}

BOOL csv2list(const char *filename, list<word_item_t>& word_list)
{
	ifstream ifs(filename, ios::in);
	if(ifs.good())
	{
		string line;
		skip_filetype_if(ifs);
		while(getline(ifs, line)) 
		{
			if(!parse_csv_line(line, word_list))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL list2csv(const list<word_item_t> &word_list, const char *filename)
{
	std::list<word_item_t>::const_iterator iter;
	ofstream ofs(filename, ios::out);

	for(iter = word_list.begin(); iter != word_list.end(); iter++)
	{
		output_to_csv(ofs, iter->name, iter->desc);
	}

	return TRUE;
}

BOOL merge_csv(const char *in_fname1, const char *in_fname2, const char *out_fname, const char *sepr)
{
	list<word_item_t> wlist;
	wlist.clear();
	strcpy(separation, sepr);
	csv2list(in_fname1, wlist);
	csv2list(in_fname2, wlist);
	list2csv(wlist, out_fname);
	return TRUE;
}
