#include "html_common.h"
#pragma comment(lib, "libxml2.lib")

string dump_xml_node(xmlNodePtr xmlnode);

void xmlGenericErrorHandler(void *ctx, const char *msg, ...)
{
	char buf[1024];
	va_list arg_ptr;
	va_start(arg_ptr, msg);
	vsnprintf(buf, sizeof(buf), msg, arg_ptr);
	va_end(arg_ptr);
	printf("Error: %s\n", buf);
	return;
}

htmlDocPtr gethtmldoc (const char *filename) {
	htmlDocPtr doc;
	//doc = htmlParseFile(filename, NULL);
	doc = htmlReadFile(filename, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);

	if (doc == NULL ) {
		fprintf(stderr,"Document not parsed successfully. \n");
		return NULL;
	}

	return doc;
}

xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlNodePtr node, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}

	if(node != NULL)
	{
		context->node = node;
	}

	result = xmlXPathEvalExpression(xpath, context);

	xmlXPathFreeContext(context);
	if (result == NULL) {
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		printf("No result\n");
		return NULL;
	}
	return result;
}

void recur_print_element_names(xmlNode * a_node)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			printf("node type: Element, name: %s\n", cur_node->name);
		}

		recur_print_element_names(cur_node->children);
	}
}

void print_element_names(xmlNode * a_node)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			printf("node type: Element, name: %s\n", cur_node->name);
		}
	}
}

xmlNodePtr get_first_node(xmlNodePtr anode)
{
	xmlNodePtr node = anode;
	while(node != NULL && node->type != XML_ELEMENT_NODE)
	{
		node = node->next;
	}
	return  node;
}

xmlNodePtr get_next_node(xmlNodePtr anode)
{
	return get_first_node(anode->next);
}

std::string dump_xml_node(xmlNodePtr xmlnode)
{
	std::string result;
	xmlBuffer *xmlbuf = xmlBufferCreate();
	xmlNodeDump(xmlbuf, xmlnode->doc, xmlnode, 0, 1);
	result = (char *)xmlBufferContent(xmlbuf);
	xmlBufferFree(xmlbuf);
	return result;
}
