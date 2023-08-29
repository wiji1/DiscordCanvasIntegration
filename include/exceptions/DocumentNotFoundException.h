#ifndef DISCORDCANVASINTEGRATION_DOCUMENTNOTFOUNDEXCEPTION_H
#define DISCORDCANVASINTEGRATION_DOCUMENTNOTFOUNDEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

class DocumentNotFoundException : public std::exception {
public:
    std::string message{"The requested document was not found!"};
    explicit DocumentNotFoundException(const std::string& what) {
        message = {what};
    }

    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }
};


#endif
