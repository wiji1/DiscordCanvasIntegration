#include "include/ImageFromHTML.h"
#include "include/UUID.h"
#include <cstdio>
#include <iostream>
#include <image.h>

void ImageFromHTML::progress_changed(wkhtmltoimage_converter * c, int p) {
    printf("%3d%%\r",p);
    fflush(stdout);
}

void ImageFromHTML::phase_changed(wkhtmltoimage_converter * c) {
    int phase = wkhtmltoimage_current_phase(c);
    printf("%s\n", wkhtmltoimage_phase_description(c, phase));
}

void ImageFromHTML::error(wkhtmltoimage_converter * c, const char * msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

void ImageFromHTML::warning(wkhtmltoimage_converter * c, const char * msg) {
    fprintf(stderr, "Warning: %s\n", msg);
}

void ImageFromHTML::init(const char* html, const std::string& file_name) {
    std::cout << "Init!" << std::endl;
    wkhtmltoimage_global_settings * gs;
    wkhtmltoimage_converter * c;
    const unsigned char * data;
    long len;

    wkhtmltoimage_init(false);

    gs = wkhtmltoimage_create_global_settings();

    wkhtmltoimage_set_global_setting(gs, "fmt", "jpeg");

    std::string converted_html {replace_unicode_escapes(html)};
    std::cout << converted_html << std::endl;

    c = wkhtmltoimage_create_converter(gs, converted_html.c_str());

    wkhtmltoimage_set_progress_changed_callback(c, progress_changed);

    wkhtmltoimage_set_phase_changed_callback(c, phase_changed);

    wkhtmltoimage_set_error_callback(c, error);

    wkhtmltoimage_set_warning_callback(c, warning);

    if(!wkhtmltoimage_convert(c)) {
        fprintf(stderr, "Conversion failed!");
    } else {
        printf("httpErrorCode: %d\n", wkhtmltoimage_http_error_code(c));
        std::cout << "test" << std::endl;

        len = wkhtmltoimage_get_output(c, &data);
        printf("%ld len\n", len);

        FILE *output_file = fopen(file_name.c_str(), "wb");
        if(output_file) {
            fwrite(data, 1, len, output_file);
            fclose(output_file);
            printf("Image saved to output.jpg\n");
        } else {
            fprintf(stderr, "Failed to open the output file for writing\n");
        }
    }

    wkhtmltoimage_destroy_converter(c);

    wkhtmltoimage_deinit();
}

std::string ImageFromHTML::replace_unicode_escapes(const std::string &input) {
    std::string result;
    size_t pos = 0;
    while(pos < input.length()) {
        size_t unicodePos = input.find("\\u", pos);
        size_t quotePos = input.find("\\\"", pos);

        if(unicodePos == std::string::npos && quotePos == std::string::npos) {
            result += input.substr(pos);
            break;
        }

        if((unicodePos < quotePos && unicodePos != std::string::npos) || quotePos == std::string::npos) {
            result += input.substr(pos, unicodePos - pos);

            std::string unicodeEscape = input.substr(unicodePos, 6);
            pos = unicodePos + 6;

            wchar_t unicodeValue = std::stoi(unicodeEscape.substr(2), nullptr, 16);
            result += static_cast<char>(unicodeValue);
        } else {
            result += input.substr(pos, quotePos - pos);

            result += "\"";

            pos = quotePos + 2;
        }
    }
    return result;
}

void ImageFromHTML::post_announcement_embed(long channel_id, const std::string &html, const std::string &title, const std::string &url, const std::string &author) {
    std::string uuid {uuid::generate_uuid_v4()};
    std::string file_name {uuid + ".jpg"};

    init(html.c_str(), file_name);

    dpp::embed image = dpp::embed();
    image.set_image("attachment://" + file_name);
    image.set_title(title);
    image.set_url(url);
    image.set_color(dpp::colors::red);
    image.set_footer(dpp::embed_footer().set_text(author));
    image.set_timestamp(time(nullptr));

    dpp::message msg(channel_id, image);
    msg.add_file(file_name, dpp::utility::read_file(file_name));

    bot->message_create(dpp::message(channel_id, image).set_channel_id(channel_id).add_file(file_name, dpp::utility::read_file(file_name)));
}
