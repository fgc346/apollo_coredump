/*
    需求：按照某个固定频率(0.5次/s)循环发布学生信息
    实现：
        1. 包含头文件
        2. 初始化cyber 框架
        3. 创建节点
        4. 创建发布者
        5. 组织并发布数据 （实现发布逻辑）
        6. 等待节点关闭，释放资源
*/

// 1. 包含头文件
#include "cyber/cyber.h"
#include "cyber/demo_base_proto/student.pb.h"

using apollo::cyber::demo_base_proto::Student;

int main(int argc, const char *argv[])
{
// 2. 初始化cyber 框架
    apollo::cyber::Init(argv[0]);
// 3. 创建节点
    auto talker_node = apollo::cyber::CreateNode("ergou");
// 4. 创建发布者
    talker_node->CreateWriter<Student>("chatter");
// 5. 组织并发布数据 （实现发布逻辑）
    //5.1 循环
    //5.2 制定频率
    apollo::cyber::Rate rate(0.5);
    uint64_t seq = 0;
    while (apollo::cyber::OK())
    {
        seq++;
        AINFO << "第" << seq << " 条数据";
        //组织数据
        auto stu_ptr = std::make_shared<Student>();
        stu_ptr->set_name("huluwa");
        stu_ptr->set_age(seq);
        stu_ptr->set_height(1.4);
        stu_ptr->add_books("yuwen");
        stu_ptr->add_books("shuxue");
        stu_ptr->add_books("yingyu");
        
        talker_node->Writer(stu_ptr);
        //休息
        rate.Sleep();
    }
// 6. 等待节点关闭，释放资源
}
