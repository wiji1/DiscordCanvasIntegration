#ifndef DISCORDCANVASINTEGRATION_ACCESSORNOTFOUNDEXCEPTION_H
#define DISCORDCANVASINTEGRATION_ACCESSORNOTFOUNDEXCEPTION_H

#include <exception>
#include <string>
#include <utility>

class AccessorNotFoundException : public std::exception {
public:
    std::string message;
    long course_id;

    explicit AccessorNotFoundException(const std::string& what, long course_id) {
        this->message = {what};
        this->course_id = {course_id};
    }

    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }
};


#endif
