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

#include "modules/common/configs/vehicle_config_helper.h"
#include "modules/planning/common/frame.h"

namespace apollo {
namespace planning {

//判断是否为非移动的障碍物
//输入参数 道路参考线信息类对象， 被判断的障碍物对象
//判断是否足够远，是否是停止的车辆，是否是被其他障碍物阻塞了？
// 通过这些条件来判断输入障碍物是否为非移动也就是静态障碍物。
bool IsNonmovableObstacle(const ReferenceLineInfo& reference_line_info,
                          const Obstacle& obstacle);

/**
 * @brief Decide whether an obstacle is a blocking one that needs to be
 *        side-passed.
 * @param The frame that contains reference_line and other info.
 * @param The obstacle of interest.
 * @param The speed threshold to tell whether an obstacle is stopped.
 * @param The minimum distance to front blocking obstacle for side-pass.
 *        (if too close, don't side-pass for safety consideration)
 * @param Whether to take into consideration that the blocking obstacle
 *        itself is blocked by others as well. In other words, if the
 *        front blocking obstacle is blocked by others, then don't try
 *        to side-pass it. (Parked obstacles are never blocked by others)
 */

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
                                  bool enable_obstacle_blocked_check);

//获取自车和障碍物之间的距离？
//输入参数frame，包含一个周期规划的所有数据
//需要计算距离的障碍物对象
double GetDistanceBetweenADCAndObstacle(const Frame& frame,
                                        const Obstacle* obstacle);

//判断障碍物是否阻塞驾驶路径
//输入参数 道路参考线类对象，障碍物类对象
// Check if the obstacle is blocking ADC's driving path (reference_line).
bool IsBlockingDrivingPathObstacle(const ReferenceLine& reference_line,
                                   const Obstacle* obstacle);

//判断是否为停止的车辆
//输入参数 道路参考线类对象，障碍物对象
//用障碍物对象的xy坐标去判断是否处在停车车道？然后又离右边道路边缘足够近？然后障碍物还属于静态障碍物，都满足则说明是停在边上的静止的车
bool IsParkedVehicle(const ReferenceLine& reference_line,
                     const Obstacle* obstacle);

}  // namespace planning
}  // namespace apollo
