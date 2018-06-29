#ifndef MYSQL_HPP
#define MYSQL_HPP

#include <mysql/mysql.h>

class MySQLCpp;
class Stmt;
class Result;

class MySQLCpp
{
public:
    MySQLCpp() :
        m_handle(mysql_init(0))
    {
    }

    ~MySQLCpp()
    {
        if(m_handle)
        {
            mysql_close(m_handle);
        }
    }

    bool connect(const std::string &host, const std::string &user,
        const std::string &password, const std::string &db, unsigned int port)
    {
        MYSQL *h = mysql_real_connect(m_handle, host.c_str(),
            user.c_str(), password.c_str(), db.c_str(), port, nullptr, 0);
        if(h)
        {
            return true;
        }
        std::cerr << "connect to database, error: " << mysql_error(m_handle)
            << ", host: " << host << ", user: " << user << ", password: "
            << password << ", db: " << db << ", port: " << port << std::endl;
        return false;
    }

    Stmt prepare(std::string s);

    Result query(std::string s);

    inline int more_results()
    {
        return mysql_more_results(m_handle);
    }

    inline int next_result();

    inline Result use_result();

private:
    MYSQL *m_handle;
};


//执行MySQLCpp.prepare时每次使用一个Stmt对应，不要使用一个Stmt对象对应多个MySQLCpp.prepare的返回值
//ERROR：
//Stmt stmt = sql.prepare("DROP TABLE IF EXISTS `students`");
//stmt = sql.prepare("TRUNCATE TABLE `students`");
//正确做法是使用两个Stmt对象，从而保证资源的正确释放
//RIGHT:
//Stmt stmt1 = sql.prepare("DROP TABLE IF EXISTS `students`");
//stmt stmt2 = sql.prepare("TRUNCATE TABLE `students`");
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

    //************************************
    // Descripts: for MySQL varchar type using std::string
    //            for MySQL int type using int
    //            for MySQL float using float
    // 对于字符串和float以及double类型，必须使用std::string和强制类型转，
    // 不能直接使用字面常量，否者内存错误或者编译类型不匹配
    // RIGHT: Stmt::execute(std::string("Zhangsan"), 10, (float)1.234);
    // ERROR: Stmt::execute(Zhangsan, 10, 1.234);
    //************************************
    template <class... Types>
    inline void execute(const Types& ... args)
    {
        _execute(0, args...);
        mysql_stmt_bind_param(stmt, params);
        mysql_stmt_execute(stmt);
    }

private:
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
    MYSQL_STMT *stmt;
    size_t count;
    MYSQL_BIND *params;
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

    Row(MYSQL_ROW row, unsigned long *lengths) :
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
    MYSQL_RES *res;
    int num_fields;
};

inline Result MySQLCpp::use_result()
{
    MYSQL_RES* result = mysql_use_result(m_handle);
    if(!result)
    {
        Result{};
    }
    return Result{result};
}

inline int MySQLCpp::next_result()
{
    int x = mysql_next_result(m_handle);
    return x;
}

Stmt MySQLCpp::prepare(std::string s)
{
    MYSQL_STMT* stmt = mysql_stmt_init(m_handle);
    int x = mysql_stmt_prepare(stmt, s.c_str(), s.size());
    if(x != 0)
    {
        return Stmt{};
    }
    return Stmt{stmt};
}

Result MySQLCpp::query(std::string s)
{
    int x = mysql_real_query(m_handle, s.c_str(), s.size());
    if(x != 0)
    {
        std::cerr << "query failed: " << mysql_error(m_handle)
            << ", command: " << s;
        return Result{};
    }
    return use_result();
}

#endif
