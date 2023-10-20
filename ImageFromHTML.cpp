#include "include/ImageFromHTML.h"
#include <cstdio>
#include <image.h>

/* Print out loading progress information */
void ImageFromHTML::progress_changed(wkhtmltoimage_converter * c, int p) {
    printf("%3d%%\r",p);
    fflush(stdout);
}

/* Print loading phase information */
void ImageFromHTML::phase_changed(wkhtmltoimage_converter * c) {
    int phase = wkhtmltoimage_current_phase(c);
    printf("%s\n", wkhtmltoimage_phase_description(c, phase));
}

/* Print a message to stderr when an error occurs */
void ImageFromHTML::error(wkhtmltoimage_converter * c, const char * msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

/* Print a message to stderr when a warning is issued */
void ImageFromHTML::warning(wkhtmltoimage_converter * c, const char * msg) {
    fprintf(stderr, "Warning: %s\n", msg);
}

/* Main method convert image */
void ImageFromHTML::init() {
    wkhtmltoimage_global_settings * gs;
    wkhtmltoimage_converter * c;
    const unsigned char * data;
    long len;

    /* Init wkhtmltoimage in graphics less mode */
    wkhtmltoimage_init(false);

    /*
     * Create a global settings object used to store options that are not
     * related to input objects, note that control of this object is parsed to
     * the converter later, which is then responsible for freeing it
     */
    gs = wkhtmltoimage_create_global_settings();

    /* We want to convert the qstring documentation page */
    wkhtmltoimage_set_global_setting(gs, "in", "http://www.google.com/");
    wkhtmltoimage_set_global_setting(gs, "fmt", "jpeg");

    /* Create the actual converter object used to convert the pages */
    c = wkhtmltoimage_create_converter(gs, NULL);

    /* Call the progress_changed function when progress changes */
    wkhtmltoimage_set_progress_changed_callback(c, progress_changed);

    /* Call the phase _changed function when the phase changes */
    wkhtmltoimage_set_phase_changed_callback(c, phase_changed);

    /* Call the error function when an error occurs */
    wkhtmltoimage_set_error_callback(c, error);

    /* Call the warning function when a warning is issued */
    wkhtmltoimage_set_warning_callback(c, warning);

    /* Perform the actual conversion */
    if (!wkhtmltoimage_convert(c))
        fprintf(stderr, "Conversion failed!");

    /* Output possible http error code encountered */
    printf("httpErrorCode: %d\n", wkhtmltoimage_http_error_code(c));

    len = wkhtmltoimage_get_output(c, &data);
    printf("%ld len\n", len);


    /* Destroy the converter object since we are done with it */
    wkhtmltoimage_destroy_converter(c);

    /* We will no longer be needing wkhtmltoimage funcionality */
    wkhtmltoimage_deinit();
}