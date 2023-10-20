#ifndef DISCORDCANVASINTEGRATION_IMAGEFROMHTML_H
#define DISCORDCANVASINTEGRATION_IMAGEFROMHTML_H

#include <cstdio>
#include <image.h>

class ImageFromHTML {
public:
    static void progress_changed(wkhtmltoimage_converter * c, int p);

/* Print loading phase information */
    static void phase_changed(wkhtmltoimage_converter * c);

/* Print a message to stderr when an error occurs */
    static void error(wkhtmltoimage_converter * c, const char * msg);

/* Print a message to stderr when a warning is issued */
    static void warning(wkhtmltoimage_converter * c, const char * msg);

/* Main method convert image */
    static void init();
};


#endif
