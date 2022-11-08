#!/usr/bin/env python3
"""
    操作student对象，
    流程：
        1. 导包
        2. 创建Student 对象
        3. 写数据
        4. 读数据
"""
# 1. 导包
from cyber.demo_base_proto.student_pb2 import Student

if __name__ == "__main__" :
    #创建Studet对象
    stu = Student()
    #写数据
    stu.name = "automan"
    stu.age = 8
    stu.height = 1.4
    stu.books.append("yuwen")
    stu.books.append("shuxue")
    stu.books.append("C++")

    #读数据
    print("name = %s; age = %d, height = %.2f" %(stu.name, stu.age, stu.height))
    for book in stu.books :
        print("book = %s" %book)