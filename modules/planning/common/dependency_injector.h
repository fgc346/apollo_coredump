/******************************************************************************
 * Copyright 2020 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#pragma once

#include "modules/common/vehicle_state/vehicle_state_provider.h"
#include "modules/planning/common/ego_info.h"
#include "modules/planning/common/frame.h"
#include "modules/planning/common/history.h"
#include "modules/planning/common/learning_based_data.h"
#include "modules/planning/common/planning_context.h"


// 关于 DependencyInjector类的注释
// DependencyInjector：依赖注入器，这是一个过于专业的名词，
// 来自软件设计模式的依赖倒置原则的一种具体实现方式，起到模块解耦作用
// DependencyInjector本质就是一个数据缓存中心，叫DataCacheCenter可能更贴切些
// DependencyInjector对象内部管理者planning模块工作过程中的实时数据和几乎全部历史数据，
// 以便于规划任务的前后帧之间的承接，以及异常处理的回溯。


namespace apollo {
namespace planning {

class DependencyInjector {
 public:
  DependencyInjector() = default;
  ~DependencyInjector() = default;

  //返回DependencyInjector类的数据成员planning_context_
  //planning_context_是PlanningContext类
  //PlanningContext类 google protobuf用法由proto文件定义的message生成
  //proto文件位于modules\planning\proto\planning_status.proto
  //该成员存放planning的状态PlanningStatus
  //planning的状态包括：bare_intersection无保护路口、change_lane变道、
  //creep_decider缓行决策等等
  PlanningContext* planning_context() {
    return &planning_context_;
  }

   //返回DependencyInjector类的数据成员frame_history_
  //Frame保存一个规划周期的所有数据，如定位/底盘，道路参考线信息，车辆状态，规划起始点
  //等
  //FrameHistory存放历史的frame信息，默认只存一帧历史信息。

  FrameHistory* frame_history() {
    return &frame_history_;
  }

  //返回DependencyInjector类的数据成员history_
  //history_存放历史frame信息以及历史障碍物状态信息？例如绕行，动静态障碍物等
  History* history() {
    return &history_;
  }
    //返回DependencyInjector类的数据成员ego_info_
  //ego_info_存放车辆自身状态信息包括一些周围环境信息，自车box盒，vehicle_state
  //以及轨迹拼接点/自车位置等信息
  EgoInfo* ego_info() {
    return &ego_info_;
  }

    //返回DependencyInjector类的数据成员vehicle_state_，就是车辆状态信息
  //定位以及底盘反馈的一些量
  apollo::common::VehicleStateProvider* vehicle_state() {
    return &vehicle_state_;
  }

  //基于学习的决策，现阶段先略过，后续掌握传统planning后可以学习下
  LearningBasedData* learning_based_data() {
    return &learning_based_data_;
  }

//  private:

//  依赖注入结构中主要有6类成员变量，分别如下：
// 1 PlanningContext planning_context_：负责planning上下文的缓存，比如是否触发重新路由的ReroutingStatus信息
// 2 History history_：负责障碍物状态的缓存，包括运动状态、决策结果。该数据与routing结果绑定，
// routing变更后会清理掉历史数据。
// 3 FrameHistory frame_history_：是一个可索引队列，负责planning的输入、输出等主要信息的缓存，以Frame类进行组织，
// 内部包含LocalView结构体（负责输入数据的融合管理)。与上述的History是不同的是，该缓存数据自模块启动后就开始缓存
// 所有的Frame对象，不受routing变动的影响。
// 4 EgoInfo ego_info_：提供车辆动、静信息，即车辆运动状态参数（轨迹、速度、加速度等）和车辆结构参数（长宽高等）
// 5 apollo::common::VehicleStateProvider vehicle_state_：车辆状态提供器，用于获取车辆实时信息
// 6 LearningBasedData learning_based_data_：基于学习的数据，用于学习建模等

  PlanningContext planning_context_;
  FrameHistory frame_history_;
  History history_;
  EgoInfo ego_info_;
  apollo::common::VehicleStateProvider vehicle_state_;
  LearningBasedData learning_based_data_;
};

}  // namespace planning
}  // namespace apollo
