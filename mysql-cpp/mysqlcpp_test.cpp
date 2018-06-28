//g++ -o test_demo mysqlcpp_test.cpp -L/usr/lib64/mysql -lmysqlclient -lstdc++

#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include <string>

int main(void)

{
    MYSQL *conn = mysql_init(NULL);
    if(!mysql_real_connect(conn, "10.0.184.1", "root", "tory_admin", "test", 3306, 0, 0))

    {
        fprintf(stderr, "mysql_real_connect: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    char *query = "insert into students (name, sex, grade) values (?, ?, ?);";
    if(mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        fprintf(stderr, "mysql_stmt_prepare: %s\n", mysql_error(conn));
        return -1;
    }

    std::string name("Wade");
    std::string sex("Male");
    int grade = 100;

    MYSQL_BIND params[3];
    memset(params, 0, sizeof(params));
    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)name.c_str();
    params[0].buffer_length = name.size();

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)sex.c_str();
    params[1].buffer_length = sex.size();

    params[2].buffer_type = MYSQL_TYPE_LONG;
    params[2].buffer = &grade;

    mysql_stmt_bind_param(stmt, params);
    mysql_stmt_execute(stmt);
    mysql_stmt_close(stmt);
    mysql_close(conn);

    return 0;
}
