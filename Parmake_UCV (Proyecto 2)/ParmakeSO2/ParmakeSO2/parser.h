#ifndef __PARSER_H
#define __PARSER_H

#define MAX_LINE_SIZE 200

void parser_parse_makefile
(
	char *,
	void (*parsed_command)   (char *, char *),
	void (*parsed_new_key)   (char *)        ,
	void (*parsed_dependency)(char *, char *)
);

#endif
