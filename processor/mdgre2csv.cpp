#include "mdgre2csv.h"
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include "../common/html_common.h"
#include "../common/common.h"
#include "../common/csv_common.h"

#include <fstream>
using namespace std;

void md_output_to_csv(ofstream &out, string &name, string &title, string &short_defines, string &defines, string &mnemonics)
{
	out << name << ",";
	out << "\"" << escape_to_csv(title)
		<< escape_to_csv(short_defines) << "\\r\\n"
		<< escape_to_csv(defines) << "\\r\\n";
	if(mnemonics.length() != 0)
	{
		out << escape_to_csv(mnemonics) << "\\r\\n";
	}
	out	<< "\"" << endl;
}

int is_space(char c)
{
	if(c == '\r' || c == '\n' || c == '\t' || c == ' ')
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int nstrtrimcat(char *dest, const char *src)
{
	const char *last;
	int cnt = 0;
	while(is_space(*src))
	{
		src++;
	}
	if(src == '\0')
	{
		return 0;
	}
	for(last = src; *last != '\0'; last++);
	last--;
	for(; last >= src && is_space(*last); last--);
	for(; *dest != '\0'; dest++);
	for(; src <= last; cnt++, src++, dest++)
	{
		*dest = *src;
	}
	*dest = '\0';
	return cnt;
}

int dumpXmlNode(xmlNodePtr anode, char *buf, int len)
{
	xmlNodePtr node;
	char tmp[256];

	if(anode->type == XML_ELEMENT_NODE)
	{
		sprintf(tmp, "<%s>", anode->name);
		len += nstrtrimcat(buf, tmp);
		for(node = anode->children; node; node = node->next)
		{
			len += dumpXmlNode(node, buf, len);
		}
		sprintf(tmp, "</%s>", anode->name);
		len += nstrtrimcat(buf, tmp);
	}
	else if(anode->type == XML_TEXT_NODE)
	{
		len += nstrtrimcat(buf, (char *)anode->content);
	}
	else
	{
		printf("Unknown node type\n");
		len = 0;
	}

	return len;
}

void parse_word_node(xmlNodePtr a_node, ofstream &ofs)
{
	xmlNodePtr cur_node;
	char buf[20480];
	string word_name;
	string word_title;
	string word_defines1;
	string word_defines2;
	string word_mnemonics;
	cur_node = get_first_node(a_node->children);
	if(cur_node->type == XML_ELEMENT_NODE && 0 == stricmp((const char *)cur_node->name, "h2"))
	{
		word_name = (char *)xmlNodeGetContent(cur_node);
		word_title = dump_xml_node(cur_node);
	}
	cur_node = get_next_node(cur_node);
	if(cur_node->type == XML_ELEMENT_NODE && 0 == stricmp((const char *)cur_node->name, "p"))
	{
		buf[0] = '\0';
		dumpXmlNode(cur_node, buf, sizeof(buf));
		word_defines1 = buf;
	}
	cur_node = get_next_node(cur_node);
	if(cur_node->type == XML_ELEMENT_NODE && 0 == stricmp((const char *)cur_node->name, "div"))
	{
		buf[0] = '\0';
		dumpXmlNode(cur_node, buf, sizeof(buf));
		word_defines2 = buf;
	}
	cur_node = get_next_node(cur_node);
	if(cur_node != NULL && cur_node->type == XML_ELEMENT_NODE 
		&& 0 == stricmp((const char *)cur_node->name, "div"))
	{
		xmlXPathObjectPtr pathoject = getnodeset (cur_node->doc, cur_node, (xmlChar*)"./div/div[@class='span9']");
		if (pathoject) {
			xmlNodeSetPtr nodeset = pathoject->nodesetval;
			word_mnemonics = "";
			sprintf(buf, "<u><strong>Mnemonics (Memory Aids) for %s</strong></u>", word_name.c_str());
			word_mnemonics += buf;
			for (int i=0; i < nodeset->nodeNr; i++) {
				string tmp = dump_xml_node(nodeset->nodeTab[i]);
				replace_all(tmp, "style=\"float:left;\"", "");
				word_mnemonics += tmp;
				word_mnemonics += "<hr />";
			}
			xmlXPathFreeObject (pathoject);
		}
	}

	md_output_to_csv(ofs, word_name, word_title, word_defines1, word_defines2, word_mnemonics);
}

void parse_mdgre_file(const char *doc_name, ofstream &ofs)
{
	xmlDocPtr doc;
	xmlChar *xpath = (xmlChar*) "/html/body/div[@id='main-content']/div/div/div[@class='span6']/div"
		"/div[@id='home-middle-content']/div[@class='row-fluid']";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr pathobj;

	xmlSetGenericErrorFunc(NULL, xmlGenericErrorHandler);

	doc = gethtmldoc(doc_name);
	pathobj = getnodeset (doc, NULL, xpath);
	if (pathobj) {
		nodeset = pathobj->nodesetval;
		for (int i=0; i < nodeset->nodeNr; i++) {
			parse_word_node(nodeset->nodeTab[i], ofs);
		}
		xmlXPathFreeObject (pathobj);
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return;
}

void mdgre2csv(const char *in_dname, const char *out_fname)
{
	char in_fname[MAX_PATH];
	ofstream ofs(out_fname, ios::out);

	for(int i = 1; i <= 787; i++)
	{
		sprintf(in_fname, "%s/MDGRE_%d.html", in_dname, i);
		parse_mdgre_file(in_fname, ofs);
	}
}
