/*
 * Copyright (C) 2014  Timothy Brown
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file xml.c
 * Routines to interact with XML.
 *
 * \ingroup XML
 * \{
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <locale.h>
#include "gettext.h"
#include "defs.h"
#include "xml.h"
#include "vcard.h"
#include "options.h"

/** Internal functions **/
static void walk_tree(xmlDocPtr, xmlNode *);

/**
 * Parse the output of curl to get the vcards.
 *
 * \parm[in] res The query result.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
parse(const char *res)
{
	size_t len      = 0;		/* Length of the result */
	xmlDocPtr doc   = NULL;		/* XML document pointer */
	xmlNodePtr node = NULL;		/* XML node pointer */

	len = strlen(res);
	doc = xmlReadMemory(res, len, "noname.xml", NULL, 0);
	if (doc == NULL) {
		warnx(_("Unable to generate an xml document"));
		return(EXIT_FAILURE);
	}

	node = xmlDocGetRootElement(doc);
	if (node == NULL) {
		warnx(_("Unable to get the root node of the xml document"));
		return(EXIT_FAILURE);
	}

	walk_tree(doc, node);

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return(EXIT_SUCCESS);
}

/**
 * Recursively walk an xml tree, when an "address-data" node is found
 * call the search function on that data.
 *
 * \parm[in] doc   The whole xml document.
 * \parm[in] node  The current node to traverse from.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
static void
walk_tree(xmlDocPtr doc, xmlNode *node)
{
	const xmlChar adr[] = "address-data";
	xmlChar *data = NULL;
	xmlNode *cur = NULL;

	for (cur = node; cur; cur = cur->next) {
		if (cur->type == XML_ELEMENT_NODE) {
				if (!xmlStrcmp(cur->name, adr)) {
					data = xmlNodeListGetString(doc,
							cur->xmlChildrenNode,
							1);
					if (options.verbose) {
						fprintf(stderr,
							_("Data:\n%s\n"),
							data);
					}
					search((const char *)data);
					xmlFree(data);
				}
		}
	walk_tree(doc, cur->children);
	}
	return;
}

/**
 * \}
 **/
