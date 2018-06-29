#include <cstring>
#include <iostream>
#include "mysqlcpp.h"

void create_test_table(MySQLCpp &sql)
{
    Stmt dropStmt = sql.prepare("DROP TABLE IF EXISTS `students`");
    if(!dropStmt)
    {
        std::cout << "drop table failed" << std::endl;
        return;
    }
    dropStmt.execute();

    Stmt createStmt = sql.prepare("CREATE TABLE `students` (\r\n"
        "`id` int NOT NULL AUTO_INCREMENT,\r\n"
        "`name` varchar(64) DEFAULT NULL,\r\n"
        "`sex` varchar(64) DEFAULT NULL,\r\n"
        "`score` int DEFAULT NULL,\r\n"
        "`height` float DEFAULT NULL\r\n,"
        "PRIMARY KEY(`id`)\r\n"
        ") ENGINE = InnoDB DEFAULT CHARSET = utf8");
    if(!createStmt)
    {
        std::cout << "create table failed" << std::endl;
        return;
    }
    createStmt.execute();
}

void clear_table_students(MySQLCpp &sql)
{
    Stmt stmt = sql.prepare("TRUNCATE TABLE `students`");
    if(!stmt)
    {
        std::cout << "clear table failed" << std::endl;
        return;
    }
    stmt.execute();
}

void regex_query(MySQLCpp &sql)
{
    Result r = sql.query("SELECT * FROM `students` WHERE `name` REGEXP 'c'");
    if(!r)
    {
        std::cout << "regex query failed" << std::endl;
        return;
    }
    while(Row row = r.next())
    {
        std::cout << std::stoi(row[0]) << "  " << row[1] << "  " << row[2] << "  " << row[3] << std::endl;
    }
}

void count_regex_like(MySQLCpp &sql)
{
    std::string points("8");
    std::string cmd = std::string("SELECT COUNT(*) AS `scores80` from `students` WHERE `score` LIKE '") +
        points + "%'";
    Result r = sql.query(cmd);
    if(!r)
    {
        std::cout << "count regex like failed" << std::endl;
        return;
    }
    std::cout << "number of score begin with 8: " << r.next()[0] << std::endl;
}

void fetch_from_test(MySQLCpp &sql)
{
    Result r = sql.query("SELECT `id`, `name`, `sex`, `score`, `height` FROM `students`");
    if(!r) return;
    while(Row row = r.next())
    {
        std::cout << std::stoi(row[0]) << "  " << row[1] << "  "
            << row[2] << "  " << row[3] << "  " << row[4] << std::endl;
    }
}

void insert_into_test(MySQLCpp &sql)
{
    Stmt stmt = sql.prepare("INSERT INTO `students` (`name`, `sex`, `score`, `height`) VALUES (?, ?, ?, ?)");
    if(!stmt)
    {
        std::cout << "insert_into_test failed" << std::endl;
        return;
    }
    stmt.execute(std::string("Tom"), std::string("Male"), 80, (float)1.70);
    stmt.execute(std::string("Jack"), std::string("Male"), 60, (float)1.78);
    stmt.execute(std::string("Lucy"), std::string("Female"), 88, (double)1.65);
}

void delete_from_test(MySQLCpp &sql)
{
    Stmt stmt = sql.prepare("DELETE FROM `students` WHERE `name` = ?");
    if(!stmt) return;
    stmt.execute(std::string("Tom"));
    //stmt.execute(std::string("Jack"));
    stmt.execute(std::string("Lucy"));
    stmt.execute(std::string("Jobs"));
}

void update_with_test(MySQLCpp &sql)
{
    Stmt stmt = sql.prepare("UPDATE `students` SET `score` = ? WHERE `name` = ?");
    if(!stmt) return;
    stmt.execute(40, std::string("Tom"));
    stmt.execute(50, std::string("Jack"));
    stmt.execute(80, std::string("Lucy"));
    stmt.execute(70, std::string("Jobs"));
}

int main(int argc, char *argv[])
{
    MySQLCpp sql{};
    if(!sql.connect("10.0.184.1", "root", "tory_admin", "test", 3306))
    {
        return 1;
    }

    create_test_table(sql);

    std::cout << "=========== insert =============" << std::endl;
    insert_into_test(sql);
    fetch_from_test(sql);

    std::cout << "============= clear ============" << std::endl;
    clear_table_students(sql);
    fetch_from_test(sql);

    std::cout << "============= insert ===========" << std::endl;
    insert_into_test(sql);
    fetch_from_test(sql);

    std::cout << "=========== regex count ========" << std::endl;
    count_regex_like(sql);

    std::cout << "========== regex serach ========" << std::endl;
    regex_query(sql);

    std::cout << "============= update ===========" << std::endl;
    update_with_test(sql);
    fetch_from_test(sql);

    std::cout << "============= delete ============" << std::endl;
    delete_from_test(sql);
    fetch_from_test(sql);

    return 0;
}
