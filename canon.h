#ifndef CANON_H
#define CANON_H

#include <termios.h>

void SetNonCanon();
void ResetCanon();
void ResetCanonicalMode(int fd, struct termios *savedattributes);
void SetNonCanonicalMode(int fd, struct termios *savedattributes);

#endif