#include <cstring>
#include <iostream>
#include "mysql.hpp"

void fetch_from_test(mysql& sql)
{
    //Result r = sql.query("SELECT `id`, `name`, `sex`, `grade` FROM `students`");
    //if(!r) return;
    //while(Row row = r.next())
    //{
    //    std::cout << std::stoi(row[0]) << "  " << row[1] << "  " << row[2] << "  " << row[3] << std::endl;
    //}
}

void insert_into_test(mysql& sql)
{
    //Stmt stmt = sql.prepare("INSERT INTO students (name, sex, grade) VALUES (?, ?, ?);");
    Stmt stmt = sql.prepare("insert into students (name, sex, grade) values (?, ?, ?);");
    //Stmt stmt = sql.prepare("insert into students (num, name) values (?, ?);");
    if(!stmt)
    {
        std::cout << "insert_into_test failed" << std::endl;
        return;
    }
    stmt.execute("Tom", "Male", 80);
}

//void delete_from_test(mysql& sql)
//{
//    Stmt stmt = sql.prepare("DELETE FROM `students` WHERE `name` = ?");
//    if(!stmt) return;
//    stmt.execute("Tom");
//    stmt.execute("Jack");
//    stmt.execute("Lucy");
//    stmt.execute("Jobs");
//}
//
//void update_with_test(mysql& sql)
//{
//    Stmt stmt = sql.prepare("UPDATE `students` SET `grade` = ? WHERE `name` = ?");
//    if(!stmt) return;
//    stmt.execute(40, "Tom");
//    stmt.execute(50, "Jack");
//    stmt.execute(90, "Lucy");
//    stmt.execute(70, "Jobs");
//}

int main()
{
    mysql sql{};
    if(!sql.connect("10.0.184.1", "root", "tory_admin", "test", 3306, 0, 0))
    {
        return 1;
    }
    std::cout << "================================" << std::endl;
    fetch_from_test(sql);
    std::cout << "================================" << std::endl;
    insert_into_test(sql);
    std::cout << "================================" << std::endl;
    fetch_from_test(sql);
    //std::cout << "================================" << std::endl;
    //update_with_test(sql);
    //std::cout << "================================" << std::endl;
    //fetch_from_test(sql);
    //std::cout << "================================" << std::endl;
    //delete_from_test(sql);
    //std::cout << "================================" << std::endl;
    //fetch_from_test(sql);
}
