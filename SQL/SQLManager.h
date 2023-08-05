#ifndef LEARNINGCPP_SQLMANAGER_H
#define LEARNINGCPP_SQLMANAGER_H
#include <string>
#include <uuid/uuid.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
#define OTL_UNICODE // Compile OTL with Unicode

#include <otlv4.h>


class SQLManager {
private:
    otl_connect db;

public:
    SQLManager(const std::string& connection_string);
    void insert(const std::string& table, uuid_t key, const std::string& data);

};


#endif
