﻿#ifndef MYSQL_HPP
#define MYSQL_HPP

#include <mysql/mysql.h>

class MySQLCpp;
class Stmt;
class Result;

class MySQLCpp
{
public:
    MySQLCpp() :
        handle(mysql_init(0))
    {
    }

    ~MySQLCpp()
    {
        if(handle)
        {
            mysql_close(handle);
        }
    }

    bool connect(const std::string &host, const std::string &user,
        const std::string &password, const std::string &db, unsigned int port)
    {
        MYSQL *h = mysql_real_connect(handle, host.c_str(),
            user.c_str(), password.c_str(), db.c_str(), port, nullptr, 0);
        if(h)
        {
            return true;
        }
        std::cerr << "Failed to connect to database: Error: " << mysql_error(handle) << std::endl;
        return false;
    }

    Stmt prepare(std::string s);

    Result query(std::string s);

    inline int more_results()
    {
        return mysql_more_results(handle);
    }

    inline int next_result();

    inline Result use_result();

private:
    MYSQL *handle;
};

class Stmt
{
public:
    Stmt() :
        stmt(0),
        count(0),
        params(0)
    {
    }

    Stmt(MYSQL_STMT *stmt) :
        stmt(stmt),
        count(mysql_stmt_param_count(stmt)),
        params(new MYSQL_BIND[count])
    {
        memset(params, 0, sizeof(MYSQL_BIND[count]));
    }

    ~Stmt()
    {
        if(stmt)
        {
            mysql_stmt_close(stmt);
        }
        if(params)
        {
            delete[] params;
        }
    }

    Stmt(const Stmt&) = delete;

    Stmt& operator=(const Stmt&) = delete;

    Stmt(Stmt &&x) :
        stmt(x.stmt),
        count(x.count),
        params(x.params),
        str_length(x.str_length)
    {
        x.stmt = 0;
        x.params = nullptr;
    }

    Stmt& operator=(Stmt &&x)
    {
        stmt = x.stmt;
        count = x.count;
        params = x.params;
        str_length = x.str_length;
        x.stmt = 0;
        x.params = nullptr;
        return *this;
    }

    operator bool()
    {
        return !!stmt;
    }

    void bind_param(int i, enum_field_types buffer_type, void *buffer,
        int buffer_length, my_bool *is_null, long unsigned int *length)
    {
        MYSQL_BIND& b = params[i];
        b.buffer_type = buffer_type;
        b.buffer = (char*)buffer;
        b.buffer_length = buffer_length;
        b.is_null = is_null;
        b.length = length;
    }

    inline void bind_param(int i, const int &x)
    {
        bind_param(i, MYSQL_TYPE_LONG, (void*)&x, 0, 0, 0);
    }

    inline void bind_param(int i, const std::string &x)
    {
        bind_param(i, MYSQL_TYPE_STRING, (void*)x.c_str(), x.size(), 0, &(params[i].buffer_length));
    }

    inline void bind_param(int i, const float &x)
    {
        bind_param(i, MYSQL_TYPE_FLOAT, (void*)&x, 0, 0, 0);
    }

    inline void bind_param(int i, const double &x)
    {
        bind_param(i, MYSQL_TYPE_DOUBLE, (void*)&x, 0, 0, 0);
    }

    template <class... Types>
    inline void execute(const Types& ... args)
    {
        _execute(0, args...);
        mysql_stmt_bind_param(stmt, params);
        mysql_stmt_execute(stmt);
    }

    template <class T, class... Types>
    inline void _execute(int n, const T &x, const Types& ... args)
    {
        bind_param(n, x);
        _execute(n + 1, args...);
    }

    // base case
    inline void _execute(int n)
    {
    }

private:
    MYSQL_STMT* stmt;
    size_t count;
    MYSQL_BIND* params;
    long unsigned int str_length;
};

class Row
{
public:
    Row() :
        row(nullptr),
        lengths(nullptr)
    {
    }

    Row(MYSQL_ROW row, unsigned long* lengths) :
        row(row),
        lengths(lengths)
    {
    }

    Row(Row &&x) :
        row(x.row),
        lengths(x.lengths)
    {
        x.row = nullptr;
        x.lengths = nullptr;
    }

    Row& operator=(Row &&x)
    {
        using std::swap;
        swap(*this, x);
        return *this;
    }

    operator bool()
    {
        return !!row;
    }

    std::string operator[](size_t n)
    {
        return std::string(row[n], lengths[n]);
    }

private:
    MYSQL_ROW row;
    unsigned long *lengths;
};

class Result
{
public:
    Result() :
        res(0),
        num_fields(0)
    {
    }

    Result(MYSQL_RES *res) :
        res(res),
        num_fields(mysql_num_fields(res))
    {
    }

    Result(Result &&r)
    {
        res = r.res;
        r.res = 0;
        num_fields = r.num_fields;
    }

    ~Result()
    {
        if(res)
        {
            mysql_free_result(res);
        }
    }

    operator bool()
    {
        return !!res;
    }

    Result& operator=(Result &&r)
    {
        mysql_free_result(res);
        res = r.res;
        num_fields = r.num_fields;
        r.res = 0;
        return *this;
    }

    Result& operator=(const Result&) = delete;

    Result(const Result&) = delete;

    Row fetch_row()
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        unsigned long* lengths = mysql_fetch_lengths(res);
        return Row{row, lengths};
    }

    inline Row next()
    {
        return fetch_row();
    }

private:
    MYSQL_RES* res;
    int num_fields;
};

inline Result MySQLCpp::use_result()
{
    MYSQL_RES* result = mysql_use_result(handle);
    if(!result)
    {
        Result{};
    }
    return Result{result};
}

inline int MySQLCpp::next_result()
{
    int x = mysql_next_result(handle);
    return x;
}

Stmt MySQLCpp::prepare(std::string s)
{
    MYSQL_STMT* stmt = mysql_stmt_init(handle);
    int x = mysql_stmt_prepare(stmt, s.c_str(), s.size());
    if(x != 0)
    {
        return Stmt{};
    }
    return Stmt{stmt};
}

Result MySQLCpp::query(std::string s)
{
    int x = mysql_real_query(handle, s.c_str(), s.size());
    if(x != 0)
    {
        std::cerr << "Error: " << mysql_error(handle) << "\n";
        return Result{};
    }
    return use_result();
}

#endif
