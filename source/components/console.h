#ifndef _CONSOLE_H_GUARD_
#define _CONSOLE_H_GUARD_

#include <stdarg.h>
#include <citro2d.h>

typedef struct {
    char *buf; // the underlying text buffer for the console
    C2D_TextBuf *textBuffer;
    uint16_t width;
    uint16_t height;
} Console;

/**
 * @brief Initialises (or reinitialises) a Console. Returns NULL on failure.
 * 
 * @param console If not NULL, a pointer to the Console to reinitialise
 * @param screen The screen to use this console on
 * @return Console * 
 */
extern Console *initConsole(Console *console, ScreenType screen);

/**
 * @brief De-initialises a Console to later be reinitalised with initConsole().
 * 
 * @param console A pointer to the Console to destroy
 */
extern void destroyConsole(Console *console);

/**
 * @brief Fully deinitialises a Console.
 * 
 * @param console A pointer to the console to destroy
 */
extern void destroyFullyConsole(Console *console);

/**
 * @brief Prints to a Console. Returns 1 on failure, else 0.
 * 
 * @param c A pointer to the output console
 * @param fmt The string format for the output
 * @param ... Any additional arguments for format specifiers
 * @return int 
 */
extern int consolePrint(Console *c, const char *fmt, ...);

#endif