# 加载py_proto_library 函数
# 参数1：函数的源文件
# 参数2：函数名
load("//tools:python_rules.bzl","py_proto_library")
#为了让其他库有权限引用student_cc库
package(default_visibility = ["//visibility:public"])
# 需要生成proto库
proto_library(
    name = "student_proto",
    srcs = [
        "student.proto",
    ],
)

#为C++生成源文件
cc_proto_library(
    name = "student_cc",
    deps = [
        ":student_proto",
    ],
)
#生成python 相关得源文件
py_proto_library(
    name = "student_py",
    deps = [
        ":student_proto",
    ],
)

# 配置C++ 调用 proto实现
cc_binary(
    name = "test_student",
    srcs = [
        "test_student.cc",
    ],
    deps = [
        ":student_cc",
    ],
)

# 配置C++ 调用 proto实现
py_binary(
    name = "test_student_py",
    srcs = [
        "test_student_py.py",
    ],
    deps = [
        ":student_py",
    ],
)