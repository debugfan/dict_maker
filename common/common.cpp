#include "common.h"

void replace_all(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

BOOL is_blank_in_line(const char c)
{
	if(c == '\t' || c == ' ')
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

const char *skip_blank(const char *p)
{
	while(*p != '\0')
	{
		if(is_crlf(*p))
		{
			p++;
			continue;
		}
		else if(is_blank_in_line(*p))
		{
			p++;
		}
		else if((unsigned char)*p == 0xa1 && (unsigned char)*(p+1) == 0xa1)
		{
			p += 2;
			continue;
		}
		else
		{
			break;
		}
	}
	return p;
}

BOOL is_crlf(const char c)
{
	if(c == '\r' || c == '\n')
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

BOOL copy_filetype_if(std::ifstream &ifs, std::ofstream &ofs)
{
	char filetype[3];
	ifs.read(filetype, sizeof(filetype));
	if((unsigned char)filetype[0] == 0xEF
		&& (unsigned char)filetype[1] == 0xBB
		&& (unsigned char)filetype[2] == 0xBF)
	{
		ofs.write(filetype, sizeof(filetype));
		return TRUE;
	}
	else
	{
		ifs.seekg (0, ifs.beg);
		return FALSE;
	}
}

BOOL skip_filetype_if(std::ifstream &ifs)
{
	char filetype[3];
	ifs.read(filetype, sizeof(filetype));
	if((unsigned char)filetype[0] == 0xEF
		&& (unsigned char)filetype[1] == 0xBB
		&& (unsigned char)filetype[2] == 0xBF)
	{
		return TRUE;
	}
	else
	{
		ifs.seekg (0, ifs.beg);
		return FALSE;
	}
}

// ['][A-z]+(['-']?[A-z])*[.]?
BOOL is_english_word_char(char c)
{
	if((c > 0x20 && c < 0x80 
		&& (isalpha(c)) || (c == '\'') || (c == '.') || (c == '-')))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL is_word_en(const char *w)
{
	for(; *w != '\0'; w++)
	{
		if(!is_english_word_char(*w))
		{
			return FALSE;
		}
	}

	return TRUE;
}

#define READ_STATE_START				0
#define READ_STATE_IN_ENGLISH_WORD		1
#define READ_STATE_CHINESE_WORD_1CHAR	2
#define READ_STATE_SPECIAL_CHAR			3

const char *read_word(char *w, int *len, const char *p)
{
	int lto = *len-1;
	int lof = 0;
	int stt = READ_STATE_START;

	for(; *p != '\0' && lof < lto; p++)
	{
		if(is_blank_in_line(*p))
		{
			break;
		}
		else
		{
			if(stt == READ_STATE_START)
			{
				if(is_english_word_char(*p))
				{
					stt = READ_STATE_IN_ENGLISH_WORD;
					w[lof++] = *p;
					continue;
				}
				else if(*p >= 128 || *p < 0)
				{
					stt = READ_STATE_CHINESE_WORD_1CHAR;
					w[lof++] = *p;
					continue;
				}
				else
				{
					stt = READ_STATE_SPECIAL_CHAR;
					w[lof++] = *p;
					p++;
					break;
				}
			}
			else if(stt == READ_STATE_IN_ENGLISH_WORD)
			{
				if(is_english_word_char(*p))
				{
					w[lof++] = *p;
					continue;
				}
				else
				{
					break;
				}
			}
			else if(stt == READ_STATE_CHINESE_WORD_1CHAR)
			{
				w[lof++] = *p;
				p++;
				break;
			}
			else
			{
				break;
			}
		}
	}

	if(lof >= lto || lof == 0)
	{
		fprintf(stderr, "Too long.\n");
		*len = 0;
		return NULL;
	}

	w[lof] = '\0';
	*len = lof;

	return p;
}
