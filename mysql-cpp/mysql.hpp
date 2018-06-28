#ifndef MYSQL_HPP
#define MYSQL_HPP

#include <mysql/mysql.h>

class mysql;
class Stmt;
class Result;

class mysql
{
public:
    mysql() : handle(mysql_init(0))
    {
    }

    ~mysql()
    {
        if(handle)
        {
            mysql_close(handle);
        }
    }

    bool connect(
        const char* host, const char* user, const char* password,
        const char* db, unsigned int port, const char* unix_socket, unsigned long client_flag)
    {
        MYSQL* h = mysql_real_connect(handle, host, user, password,
            db, port, unix_socket, client_flag);
        if(h) return true;
        std::cerr << "Failed to connect to database: Error: " << mysql_error(handle) << "\n";
        return false;
    }

    Stmt   prepare(std::string s);
    Result query(std::string s);

    inline
        int more_results()
    {
        return mysql_more_results(handle);
    }

    inline int next_result();
    inline Result use_result();
private:
    MYSQL* handle;
};

class Stmt
{
private:
    MYSQL_STMT* stmt;
    size_t count;
    MYSQL_BIND* params;
    long unsigned int str_length;
public:
    Stmt() : stmt(0), count(0), params(0)
    {
    }
    Stmt(MYSQL_STMT* stmt)
        : stmt(stmt), count(mysql_stmt_param_count(stmt)), params(new MYSQL_BIND[count])
    {
        std::cout << "Stmt count: " << count << std::endl;
        memset(params, 0, sizeof(MYSQL_BIND[count]));
    }

    ~Stmt()
    {
        if(stmt) mysql_stmt_close(stmt);
        delete[] params;
    }

    Stmt(const Stmt&) = delete;
    Stmt& operator=(const Stmt&) = delete;

    Stmt(Stmt&& x) : stmt(x.stmt), count(x.count), params(x.params), str_length(x.str_length)
    {
        x.stmt = 0;
        x.params = nullptr;
    }

    Stmt& operator=(Stmt&& x)
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


    void bind_param(int i, enum_field_types buffer_type, void* buffer,
        int buffer_length, my_bool* is_null, long unsigned int* length)
    {
        std::cout << "execute bind param: " << i << std::endl;
        MYSQL_BIND& b = params[i];
        b.buffer_type = buffer_type;
        b.buffer = (char*)buffer;
        std::cout << "b.buffer content: " << *(int*)buffer << std::endl;
        b.buffer_length = buffer_length;
        b.is_null = is_null;
        b.length = length;
    }

    inline void bind_param(int i, const int& x)
    {
        std::cout << "bind_param int: (" << i << ", " << x << ")" << std::endl;
        //bind_param(i, MYSQL_TYPE_LONG, (char*)&x, 0, 0, 0);
        params[i].buffer_type = MYSQL_TYPE_LONG;
        params[i].buffer = (void*)&x;
        std::cout << "bind_param int end: " << *(int*)params[i].buffer << std::endl;
    }

    inline void bind_param(int i, const std::string& x)
    {
        std::cout << "bind_param string: (" << i << ", " << x << ")" << std::endl;
        //bind_param(i, MYSQL_TYPE_STRING, (char*)x.c_str(), x.size(), 0, 0/*&(params[i].buffer_length)*/);
        params[i].buffer_type = MYSQL_TYPE_STRING;
        params[i].buffer = (void*)x.c_str();
        params[i].buffer_length = x.size();
    }

    template <class... Types>
    inline void execute(const Types& ... args)
    {
        std::cout << "Types count: " << sizeof...(Types) << ", args counts: " << sizeof...(args) << std::endl;
        _execute(0, args...);
        for(auto i = 0; i < count; ++i)
        {
            if(params[i].buffer_type == MYSQL_TYPE_STRING)
            {
                std::cout << "index：" << i << ", string parmas: " << std::string((char*)params[i].buffer, params[i].buffer_length) << std::endl;
            }
            if(params[i].buffer_type == MYSQL_TYPE_LONG)
            {
                std::cout << "index：" << i << ", long parmas: " << *(int*)params[i].buffer << std::endl;
            }
        }
        mysql_stmt_bind_param(stmt, params);
        mysql_stmt_execute(stmt);
    }

    template <class T, class... Types>
    inline void _execute(int n, const T& x, const Types& ... args)
    {
        //std::cout << "_execute(int n, const T& x, Types ... args): (" << n << ", " << x <<
        //    ", args count: " << sizeof...(args) << ")" << std::endl;
        bind_param(n, x);
        _execute(n + 1, args...);
    }

    // base case
    inline
        void _execute(int n)
    {
        //std::cout << "----------------end bind params--------------------" << std::endl;
    }
};

class Row
{
private:
    MYSQL_ROW row;
    unsigned long *lengths;
public:
    Row() : row(nullptr), lengths(nullptr)
    {
    }
    Row(MYSQL_ROW row, unsigned long* lengths) : row(row), lengths(lengths)
    {
    }

    Row(Row&& x) : row(x.row), lengths(x.lengths)
    {
        x.row = nullptr;
        x.lengths = nullptr;
    }

    Row& operator=(Row&& x)
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
};

class Result
{
private:
    MYSQL_RES* res;
    int num_fields;
public:
    Result() : res(0), num_fields(0)
    {
    }
    Result(MYSQL_RES* res) : res(res), num_fields(mysql_num_fields(res))
    {
    }
    Result(Result&& r)
    {
        res = r.res;
        r.res = 0;

        num_fields = r.num_fields;
    }

    ~Result()
    {
        if(res) mysql_free_result(res);
    }

    operator bool()
    {
        return !!res;
    }

    Result& operator=(Result&& r)
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

    inline
        Row next()
    {
        return fetch_row();
    }
};

inline
Result mysql::use_result()
{
    MYSQL_RES* result = mysql_use_result(handle);
    if(!result)
    {
        Result{};
    }
    return Result{result};
}

inline
int mysql::next_result()
{
    int x = mysql_next_result(handle);
    return x;
}

Stmt mysql::prepare(std::string s)
{
    MYSQL_STMT* stmt = mysql_stmt_init(handle);
    int x = mysql_stmt_prepare(stmt, s.c_str(), s.size());
    if(x != 0)
    {
        return Stmt{};
    }
    return Stmt{stmt};
}

Result mysql::query(std::string s)
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
