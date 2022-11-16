/*
    演示C++中 protobuf 的基本读写使用
    操作student对象，
    流程：
        1.包含头文件
        2. 创建Student 对象
        3. 写数据
        4. 读数据
*/

#include "cyber/demo_base_proto/student.pb.h"
#include <iostream>
int main(int argc, const char *argv[])
{
    //创建 Student 对象
    apollo::cyber::demo_base_proto::Student stu;
    // 数据写
    //一般情况下，调用的是 set_xxx 函数
    stu.set_name("zhangsan");
    stu.set_age(18);
    stu.set_height(1.75);
    stu.add_books("yuwen");
    stu.add_books("c++");
    stu.add_books("Python");

    //数据读
    std::string name = stu.name();
    uint64_t age = stu.age();
    double height = stu.height();
    std::cout << "name : " <<  name << " age: " << age << " height: " << height << std::endl;
    for (int i = 0; i < stu.books_size(); i++)
    {
        std::cout << stu.books(i) << ", ";
    }
    std::cout << std::endl;
    return 0;
}