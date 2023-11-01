#ifndef DISCORDCANVASINTEGRATION_IMAGEFROMHTML_H
#define DISCORDCANVASINTEGRATION_IMAGEFROMHTML_H

#include <cstdio>
#include <image.h>
#include <string>
#include <dpp/dpp.h>

extern std::unique_ptr<dpp::cluster> bot;

class ImageFromHTML {
public:
    static void progress_changed(wkhtmltoimage_converter *c, int p);

    static void phase_changed(wkhtmltoimage_converter *c);

    static void error(wkhtmltoimage_converter *c, const char *msg);

    static void warning(wkhtmltoimage_converter *c, const char *msg);

    static dpp::task<void> init(const char *html, const std::string &file_name);

    static std::string replace_unicode_escapes(const std::string &input);

    static dpp::task<void> post_announcement_embed(long channel_id, const std::string &file_name, const std::string &title, const std::string &url,
                            const std::string &author);

    static dpp::task<void> post_assignment_embed(long channel_id, const std::string &file_name, const std::string &title, const std::string &url,
                          const std::string &due, int points);

};
#endif
