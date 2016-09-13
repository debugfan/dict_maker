#include "oed2csv.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLparser.h>
#include "mdgre2csv.h"
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include "../common/common.h"
#include "../common/html_common.h"
#include "../common/csv_common.h"

#include <fstream>
#include <list>
using namespace std;

void oed_output_to_csv(ofstream &out, string &name, string &title, string &desc)
{
	out << name << ",";
	out << "\"" << escape_to_csv(title + "\r\n")
		<< escape_to_csv(desc + "\r\n")
		<< "\"" << endl;
}

string read_name_from_oed_dt(xmlNodePtr a_node)
{
	xmlNodePtr node;
	node = get_first_node(a_node->children);
	if(0 == stricmp((const char *)node->name, "a"))
	{
		return (const char *)xmlNodeGetContent(node);
	}
	else
	{
		return "";
	}
}

void parse_oed_dl(xmlNodePtr a_node, ofstream &ofs)
{
	xmlNodePtr node;
	string name;
	string title;
	string desc;
	node = get_first_node(a_node);
	while(node != NULL)
	{
		if(0 == stricmp((char *)node->name, "dt"))
		{
			name = read_name_from_oed_dt(node);
			if(name.length() == 0)
			{
				fprintf(stderr, "read_name_from_oed_dt failed");
			}
			title = dump_xml_node(node);
			replace_all(title, "href=\"/", "href=\"http://www.etymonline.com/");
		}
		else
		{
			fprintf(stderr, "Unknown error\n");
		}
		node = get_next_node(node);
		if(0 == stricmp((const char *)node->name, "dd"))
		{
			desc = dump_xml_node(node);
			replace_all(desc, "href=\"/", "href=\"http://www.etymonline.com/");
		}
		else
		{
			fprintf(stderr, "Unknown error\n");
		}
		oed_output_to_csv(ofs, name, title, desc);
		node = get_next_node(node);
	}
}

void parse_oed_file(const char *doc_name, ofstream &ofs)
{
	xmlDocPtr doc;
	xmlChar *xpath = (xmlChar*) "/html/body/div/div/dl";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr pathobj;

	xmlSetGenericErrorFunc(NULL, xmlGenericErrorHandler);

	doc = gethtmldoc(doc_name);
	pathobj = getnodeset (doc, NULL, xpath);
	if (pathobj) {
		nodeset = pathobj->nodesetval;
		for (int i=0; i < nodeset->nodeNr; i++) {
			parse_oed_dl(nodeset->nodeTab[i]->children, ofs);
		}
		xmlXPathFreeObject (pathobj);
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return;
}

int list_dir(list<string> &filelist, const char *dirname, const char *pattern)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError;
	LPTSTR DirSpec;
	size_t length_of_arg;
	INT retval;

#define BUFSIZE MAX_PATH
	DirSpec = (LPTSTR) malloc (BUFSIZE);

	if( DirSpec == NULL )
	{
		printf( "Insufficient memory available\n" );
		retval = 1;
		goto Cleanup;
	}

	// Check that the input is not larger than allowed.
	StringCbLength(dirname, BUFSIZE, &length_of_arg);

	if (length_of_arg > (BUFSIZE - 2))
	{
		_tprintf(TEXT("Input directory is too large.\n"));
		retval = 3;
		goto Cleanup;
	}

	//_tprintf (TEXT("Target directory is %s.\n"), dirname);

	// Prepare string for use with FindFile functions.  First, 
	// copy the string to a buffer, then append '\*' to the 
	// directory name.
	StringCbCopyN (DirSpec, BUFSIZE, dirname, length_of_arg+1);
	StringCbCatN (DirSpec, BUFSIZE, TEXT("\\"), 1*sizeof(TCHAR));
	StringCbCatN (DirSpec, BUFSIZE, pattern, strlen(pattern));

	// Find the first file in the directory.
	hFind = FindFirstFile(DirSpec, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		_tprintf (TEXT("Invalid file handle. Error is %u.\n"), 
			GetLastError());
		retval = (-1);
	} 
	else 
	{
		//_tprintf (TEXT("First file name is: %s\n"), 
		//	FindFileData.cFileName);
		filelist.push_back(FindFileData.cFileName);

		// List all the other files in the directory.
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			//_tprintf (TEXT("Next file name is: %s\n"), 
			//	FindFileData.cFileName);
			filelist.push_back(FindFileData.cFileName);
		}

		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES) 
		{
			_tprintf (TEXT("FindNextFile error. Error is %u.\n"), 
				dwError);
			retval = (-1);
			goto Cleanup;
		}
	}
	retval  = 0;

Cleanup:
	free(DirSpec);
	return retval;
}

void oed2csv(const char *in_dname, const char *out_fname)
{
	char full_dname[MAX_PATH];
	ofstream ofs(out_fname, ios::out);
	memset(full_dname, 0, sizeof(full_dname));
	GetCurrentDirectory(sizeof(full_dname), full_dname);
	strcat(full_dname, "\\");
	strcat(full_dname, in_dname);
	list<string> filelist;
	char match_name[MAX_PATH];
	char filepath[MAX_PATH];

	for(char c = 'a'; c <= 'z'; c++)
	{
		filelist.clear();
		sprintf(match_name, "OED_%c*", c);
		list_dir(filelist, full_dname, match_name);
		for(list<string>::iterator iter = filelist.begin(); iter != filelist.end(); iter++)
		{
			sprintf(filepath, "%s\\%s", full_dname, iter->c_str());
			parse_oed_file(filepath, ofs);
		}
	}
}