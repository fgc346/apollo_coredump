#! /usr/bin/env bash
###############################################################################
# Copyright 2020 The Apollo Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###############################################################################

# Ref: http://www.linuxfromscratch.org/blfs/view/svn/postlfs/profile.html
# Written for Beyond Linux From Scratch
# by James Robertson <jameswrobertson@earthlink.net>
# modifications by Dagmar d'Surreal <rivyqntzne@pbzpnfg.arg>

# Functions to help us manage paths.  Second argument is the name of the
# path variable to be modified (default: PATH)
pathremove() {
    local IFS=':'
    local NEWPATH
    local DIR
    # [fgc add] ${varname:-word} 如果变量varname存在且不为空，则返回它的值，否则返回word
    # 这里返回默认值 PATH
    # 2 表示脚本的第二个参数
    # 如果脚本的第二个参数存在，则PATHVARIABLE = $2,否则 PATHVARIABLE = PATH
    # PATH 由冒号分开的目录列表，当输入可执行程序名后，会搜索这个目录列表
    local PATHVARIABLE=${2:-PATH}
    # echo "[fgc,add], PATHVARIABLE=${PATHVARIABLE}"
    # 
    for DIR in ${!PATHVARIABLE}; do
        if [ "$DIR" != "$1" ]; then
            # ${varname:+word} 含义 如果变量名存在且不为空，则返回word，否则返回空值。它的目的是测试变量是否存在
            NEWPATH=${NEWPATH:+$NEWPATH:}$DIR
        fi
    done
    # echo "[fgc,add], NEWPATH=${NEWPATH}"
    export $PATHVARIABLE="$NEWPATH"
}

pathprepend() {
    pathremove $1 $2
    local PATHVARIABLE=${2:-PATH}
    # echo "[fgc,add], !PATHVARIABLE"
    # echo "[fgc,add, \${!PATHVARIABLE} = ${!PATHVARIABLE}"
    # [fgc,add] PATHVARIABLE 如果是一个变量，!PATHVARIABLE即为PATH
    # ${var:+newstring},如果var不为空，就返回newstring；如果var为空，就返回空值
    # 这个函数的目的就是把当前路径加入到PATH路径的最前面，以分号：分隔开
    export $PATHVARIABLE="$1${!PATHVARIABLE:+:${!PATHVARIABLE}}"
}

pathappend() {
    pathremove $1 $2
    local PATHVARIABLE=${2:-PATH}
    export $PATHVARIABLE="${!PATHVARIABLE:+${!PATHVARIABLE}:}$1"
}

# [fgc,add] 函数也能像环境变量一样用export到处，如此，函数的作用域就可以扩展到子进程中。
# export -f fname
export -f pathremove pathprepend pathappend
