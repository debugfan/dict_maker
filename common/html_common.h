#ifndef HTML_COMMON_H
#define HTML_COMMON_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <string>
using std::string;

void xmlGenericErrorHandler(void *ctx, const char *msg, ...);
htmlDocPtr gethtmldoc (const char *filename);
xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlNodePtr node, xmlChar *xpath);
xmlNodePtr get_first_node(xmlNodePtr anode);
xmlNodePtr get_next_node(xmlNodePtr anode);
std::string dump_xml_node(xmlNodePtr xmlnode);

#endif