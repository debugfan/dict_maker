// dict_converter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "csv2ycd.h"
#include "csv2md.h"
#include "../common/csv_common.h"
#include "../common/sys_common.h"

#define DICT_FORMAT_MDICT   0
#define DICT_FORMAT_YCD   1

bool is_csv(const char *filename)
{
    std::string ext;
    ext = extract_filename_extension(filename);
    if (0 == strcmp(ext.c_str(), ".csv"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void format_csv_files(const char *dir, int format_type)
{
    std::list<std::string> files;
    std::list<std::string>::iterator iter;
    char input[MAX_PATH];
    char output[MAX_PATH];
    char output_dir[MAX_PATH];
    char output_postfix[64];
    BOOL(*format_func)(const char *input, const char *output);
    if (format_type == DICT_FORMAT_YCD)
    {
        strcpy(output_dir, dir);
        strcat(output_dir, "\\ycd");
        strcpy(output_postfix, ".ycd.txt");
        format_func = csv2ycd;
    }
    else
    {
        strcpy(output_dir, dir);
        strcat(output_dir, "\\mdict");
        strcpy(output_postfix, ".mdict.txt");
        format_func = csv2md;
    }
    create_dir_safely(output_dir);
    list_files(files, dir);
    for (iter = files.begin(); iter != files.end(); iter++)
    {
        if (is_csv(iter->c_str()))
        {
            sprintf(input, "%s\\%s", dir, iter->c_str());
            sprintf(output, "%s\\%s%s",
                output_dir,
                remove_filename_extension(iter->c_str()).c_str(),
                output_postfix);
             printf("input: %s, output: %s\n", input, output);
             format_func(input, output);
        }
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc > 0 && 0 == strcmp(argv[0], "--ycd"))
    {
        format_csv_files("..\\data", DICT_FORMAT_YCD);
    }
    else
    {
        format_csv_files("..\\data", DICT_FORMAT_MDICT);
    }

	return 0;
}
