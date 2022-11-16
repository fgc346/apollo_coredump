/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
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

#include "modules/planning/common/obstacle_blocking_analyzer.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "modules/common/configs/vehicle_config_helper.h"
#include "modules/common/util/point_factory.h"
#include "modules/map/hdmap/hdmap_util.h"
#include "modules/planning/common/frame.h"
#include "modules/planning/common/planning_gflags.h"

namespace apollo {
namespace planning {

using apollo::common::VehicleConfigHelper;
using apollo::hdmap::HDMapUtil;

//这类定义了两个阈值，自车距离阈值35m，障碍物距离阈值15m，干什么用的？
constexpr double kAdcDistanceThreshold = 35.0;  // unit: m
constexpr double kObstaclesDistanceThreshold = 15.0;

//判断是否为非移动的障碍物
//输入参数 道路参考线信息类对象， 被判断的障碍物对象
//判断是否足够远，是否是停止的车辆，是否是被其他障碍物阻塞了？通过这些条件来判断输入障碍物是否为非移动也就是静态障碍物。
bool IsNonmovableObstacle(const ReferenceLineInfo& reference_line_info,
                          const Obstacle& obstacle) {
  // Obstacle is far away.
  const SLBoundary& adc_sl_boundary = reference_line_info.AdcSlBoundary();
  if (obstacle.PerceptionSLBoundary().start_s() >
      adc_sl_boundary.end_s() + kAdcDistanceThreshold) {
    ADEBUG << " - It is too far ahead and we are not so sure of its status.";
    return false;
  }

  //如果障碍物的感知SL边界的起始s比自车SL边界终点s+35m还要大
  //就说明障碍物离自车非常远？无需考虑直接返回false?
  // Obstacle is parked obstacle.
  if (IsParkedVehicle(reference_line_info.reference_line(), &obstacle)) {
    ADEBUG << "It is Parked and NON-MOVABLE.";
    return true;
  }

  // Obstacle is blocked by others too.
  for (const auto* other_obstacle :
       reference_line_info.path_decision().obstacles().Items()) {
    if (other_obstacle->Id() == obstacle.Id()) {
      continue;
    }
    if (other_obstacle->IsVirtual()) {
      continue;
    }
    if (other_obstacle->PerceptionSLBoundary().start_l() >
            obstacle.PerceptionSLBoundary().end_l() ||
        other_obstacle->PerceptionSLBoundary().end_l() <
            obstacle.PerceptionSLBoundary().start_l()) {
      // not blocking the backside vehicle
      continue;
    }
    double delta_s = other_obstacle->PerceptionSLBoundary().start_s() -
                     obstacle.PerceptionSLBoundary().end_s();
    if (delta_s < 0.0 || delta_s > kObstaclesDistanceThreshold) {
      continue;
    }

    // TODO(All): Fix the segmentation bug for large vehicles, otherwise
    // the follow line will be problematic.
    ADEBUG << " - It is blocked by others, and will move later.";
    return false;
  }

  ADEBUG << "IT IS NON-MOVABLE!";
  return true;
}

// This is the side-pass condition for every obstacle.
// TODO(all): if possible, transform as many function parameters into GFLAGS.
// 判断一个障碍物是否阻塞路径，需要自车绕行
/**
 * @brief 判断一个障碍物是否阻塞路径，需要自车绕行？
 * @param frame包含了一个周期内规划所有信息包括道路参考线等
 * @param 感兴趣的障碍物obstacle
 * @param 判断一个障碍物是否停止的速度阈值block_obstacle_min_speed
 * @param 到前方阻塞障碍物需要绕行的最小距离min_front_sidepass_distance
 *        如果太近了，出于安全考虑，不绕行。
 * @param 是否要考虑被阻塞障碍物本身被其他障碍物阻塞了enable_obstacle_blocked_check
 *        如果前方的障碍物也是被前面的障碍物阻塞，那么就不要尝试绕行它？
 */
bool IsBlockingObstacleToSidePass(const Frame& frame, const Obstacle* obstacle,
                                  double block_obstacle_min_speed,
                                  double min_front_sidepass_distance,
                                  bool enable_obstacle_blocked_check) {
  // 获取必要的信息
  //frame里的第一条参考线信息类对象
  // Get the necessary info.
  const auto& reference_line_info = frame.reference_line_info().front();
  // 道路参考线
  const auto& reference_line = reference_line_info.reference_line();
  //自车SL边界
  const SLBoundary& adc_sl_boundary = reference_line_info.AdcSlBoundary();
  //路径决策类对象
  const PathDecision& path_decision = reference_line_info.path_decision();
  ADEBUG << "Evaluating Obstacle: " << obstacle->Id();

  // Obstacle is virtual.
  if (obstacle->IsVirtual()) {
    ADEBUG << " - It is virtual.";
    return false;
  }

  // Obstacle is moving.
  // 如果障碍物非静态，或者障碍物的速度大于判断停车的速度阈值，也认为是非静态障碍物
  //无需绕行
  if (!obstacle->IsStatic() || obstacle->speed() > block_obstacle_min_speed) {
    ADEBUG << " - It is non-static.";
    return false;
  }

  // Obstacle is behind ADC.
  if (obstacle->PerceptionSLBoundary().start_s() <= adc_sl_boundary.end_s()) {
    ADEBUG << " - It is behind ADC.";
    return false;
  }

  // Obstacle is far away.
  //障碍物的SL边界的起始s 比 自车SL边界的终点s + 15m就认为障碍物足够远，无需绕行
  static constexpr double kAdcDistanceSidePassThreshold = 15.0;
  if (obstacle->PerceptionSLBoundary().start_s() >
      adc_sl_boundary.end_s() + kAdcDistanceSidePassThreshold) {
    ADEBUG << " - It is too far ahead.";
    return false;
  }

  // Obstacle is too close.
  // 障碍物太近了
  //如果自车SL边界的终点s + 最小绕行距离阈值 > 输入障碍物SL边界的起点s，出于安全考虑，放弃绕行
  if (adc_sl_boundary.end_s() + min_front_sidepass_distance >
      obstacle->PerceptionSLBoundary().start_s()) {
    ADEBUG << " - It is too close to side-pass.";
    return false;
  }

  // 障碍物没有阻塞我们的驾驶路径，直接返回false，调用IsBlockingDrivingPathObstacle函数进行判断
  // Obstacle is not blocking our path.
  if (!IsBlockingDrivingPathObstacle(reference_line, obstacle)) {
    ADEBUG << " - It is not blocking our way.";
    return false;
  }

  // 障碍物也被其他障碍物阻塞了，也放弃绕行
  //如果打开障碍物被其他障碍物阻塞的检查开关 且 输入障碍物不是个静止车辆
  // Obstacle is blocked by others too.
  if (enable_obstacle_blocked_check &&
      !IsParkedVehicle(reference_line, obstacle)) {
    for (const auto* other_obstacle : path_decision.obstacles().Items()) {
      if (other_obstacle->Id() == obstacle->Id()) {
        continue;
      }
      if (other_obstacle->IsVirtual()) {
        continue;
      }
      if (other_obstacle->PerceptionSLBoundary().start_l() >
              obstacle->PerceptionSLBoundary().end_l() ||
          other_obstacle->PerceptionSLBoundary().end_l() <
              obstacle->PerceptionSLBoundary().start_l()) {
        // not blocking the backside vehicle
        continue;
      }
      double delta_s = other_obstacle->PerceptionSLBoundary().start_s() -
                       obstacle->PerceptionSLBoundary().end_s();
      if (delta_s < 0.0 || delta_s > kAdcDistanceThreshold) {
        continue;
      }

      // TODO(All): Fix the segmentation bug for large vehicles, otherwise
      // the follow line will be problematic.
      ADEBUG << " - It is blocked by others, too.";
      return false;
    }
  }

  ADEBUG << "IT IS BLOCKING!";
  return true;
}

double GetDistanceBetweenADCAndObstacle(const Frame& frame,
                                        const Obstacle* obstacle) {
  const auto& reference_line_info = frame.reference_line_info().front();
  const SLBoundary& adc_sl_boundary = reference_line_info.AdcSlBoundary();
  double distance_between_adc_and_obstacle =
      obstacle->PerceptionSLBoundary().start_s() - adc_sl_boundary.end_s();
  return distance_between_adc_and_obstacle;
}

bool IsBlockingDrivingPathObstacle(const ReferenceLine& reference_line,
                                   const Obstacle* obstacle) {
  const double driving_width =
      reference_line.GetDrivingWidth(obstacle->PerceptionSLBoundary());
  const double adc_width =
      VehicleConfigHelper::GetConfig().vehicle_param().width();
  ADEBUG << " (driving width = " << driving_width
         << ", adc_width = " << adc_width << ")";
  //如果驾驶宽度 > 自车宽度 + 0.3m(左右一起0.3m障碍物缓冲) +  0.1m(绕行缓冲)，那么则说明障碍物没有阻塞我们的驾驶路径
  //FLAGS_static_obstacle_nudge_l_buffer google gflags的老用法
  //FLAGS_代表去modules\planning\common\planning_gflags.cc取出static_obstacle_nudge_l_buffer的值，默认为0.3m
  if (driving_width > adc_width + FLAGS_static_obstacle_nudge_l_buffer +
                          FLAGS_side_pass_driving_width_l_buffer) {
    // TODO(jiacheng): make this a GFLAG:
    // side_pass_context_.scenario_config_.min_l_nudge_buffer()
    ADEBUG << "It is NOT blocking our path.";
    return false;
  }

  ADEBUG << "It is blocking our path.";
  return true;
}

// 判断是否为停止的车辆
//输入参数 道路参考线类对象，障碍物对象
//用障碍物对象的xy坐标去判断是否处在停车车道？然后又离右边道路边缘足够近？然后障碍物还属于静态障碍物，都满足则说明是停在边上的静止的车
bool IsParkedVehicle(const ReferenceLine& reference_line,
                     const Obstacle* obstacle) {
  if (!FLAGS_enable_scenario_side_pass_multiple_parked_obstacles) {
    return false;
  }
  double road_left_width = 0.0;
  double road_right_width = 0.0;
  double max_road_right_width = 0.0;
  reference_line.GetRoadWidth(obstacle->PerceptionSLBoundary().start_s(),
                              &road_left_width, &road_right_width);
  max_road_right_width = road_right_width;
  reference_line.GetRoadWidth(obstacle->PerceptionSLBoundary().end_s(),
                              &road_left_width, &road_right_width);
  max_road_right_width = std::max(max_road_right_width, road_right_width);
  bool is_at_road_edge = std::abs(obstacle->PerceptionSLBoundary().start_l()) >
                         max_road_right_width - 0.1;

  std::vector<std::shared_ptr<const hdmap::LaneInfo>> lanes;
  auto obstacle_box = obstacle->PerceptionBoundingBox();
  HDMapUtil::BaseMapPtr()->GetLanes(
      common::util::PointFactory::ToPointENU(obstacle_box.center().x(),
                                             obstacle_box.center().y()),
      std::min(obstacle_box.width(), obstacle_box.length()), &lanes);
  bool is_on_parking_lane = false;
  if (lanes.size() == 1 &&
      lanes.front()->lane().type() == apollo::hdmap::Lane::PARKING) {
    is_on_parking_lane = true;
  }

  bool is_parked = is_on_parking_lane || is_at_road_edge;
  return is_parked && obstacle->IsStatic();
}

}  // namespace planning
}  // namespace apollo
