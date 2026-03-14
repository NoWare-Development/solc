#ifndef __SOLC_COLOR_H__
#define __SOLC_COLOR_H__

typedef const char *esccolor_t;
typedef const char *escgraphics_t;

#define ESC_RESET "\033[0m"

#define ESCCOLOR_BLACK "\033[30m"
#define ESCCOLOR_RED "\033[31m"
#define ESCCOLOR_GREEN "\033[32m"
#define ESCCOLOR_YELLOW "\033[33m"
#define ESCCOLOR_BLUE "\033[34m"
#define ESCCOLOR_MAGENTA "\033[35m"
#define ESCCOLOR_CYAN "\033[36m"
#define ESCCOLOR_WHITE "\033[37m"
#define ESCCOLOR_DEFAULT "\033[39m"

#define ESCGRAPHICS_BOLD "\033[1m"
#define ESCGRAPHICS_DIM "\033[2m"
#define ESCGRAPHICS_ITALIC "\033[3m"
#define ESCGRAPHICS_UNDERLINE "\033[4m"
#define ESCGRAPHICS_BLINK "\033[5m"
#define ESCGRAPHICS_INVERSE "\033[7m"
#define ESCGRAPHICS_HIDDEN "\033[8m"
#define ESCGRAPHICS_STRIKETHROUGH "\033[9m"

#endif // __SOLC_COLOR_H__
