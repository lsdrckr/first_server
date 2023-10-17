#ifndef __HTTP_H__
#define __HTTP_H__

#include <stdio.h>
#include <stdlib.h>

void sendHtml(FILE *stream, char* htmlPath);
void requestHandler(FILE* stream);

#endif
