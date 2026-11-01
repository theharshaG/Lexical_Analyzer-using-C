#ifndef SYNTAX_H
#define SYNTAX_H

#include "lexer.h"
void checkDelimiters();
int isHeaderPresent(const char *header);
void checkRequiredHeaders();
int isDataType(const char *word);
void checkSemicolon();
void storeTokens();
int isFunctionCall(int index);
int isFunctionDefinition(int index);
int findClosingParen(int index);
int isAssignment(int index);

#endif