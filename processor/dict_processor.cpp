// dict_converter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mdgre2csv.h"
#include "oed2csv.h"

void raw2csv()
{
    mdgre2csv("../data/raw/MDGRE", "../data/mdgre.csv");
    oed2csv("../data/raw/OED", "../data/oed.csv");
}

int _tmain(int argc, _TCHAR* argv[])
{
    raw2csv();
	return 0;
}
