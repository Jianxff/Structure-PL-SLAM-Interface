/**
 * This file is part of Structure PLP-SLAM, originally from OpenVSLAM.
 *
 * Copyright 2022 DFKI (German Research Center for Artificial Intelligence)
 * Modified by Fangwen Shu <Fangwen.Shu@dfki.de>
 *
 * If you use this code, please cite the respective publications as
 * listed on the github repository.
 *
 * Structure PLP-SLAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Structure PLP-SLAM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Structure PLP-SLAM. If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/optimize/g2o/landmark_vertex_container.h"

namespace PLSLAM
{
    namespace optimize
    {
        namespace g2o
        {

            landmark_vertex_container::landmark_vertex_container(const unsigned int offset,
                                                                 const unsigned int num_reserve)
                : offset_(offset)
            {
                vtx_container_.reserve(num_reserve);
            }

            landmark_vertex *landmark_vertex_container::create_vertex(const unsigned int id,
                                                                      const Vec3_t &pos_w,
                                                                      const bool is_constant)
            {
                // Create vertex
                const auto vtx_id = offset_ + id;
                auto vtx = new landmark_vertex();
                vtx->setId(vtx_id);
                vtx->setEstimate(pos_w); // set the value of parameters need to be optimized, here, initialized by the value of pos_w
                vtx->setFixed(is_constant);

                // FW: The reason that we need to set marginalized only for point landmark
                //  https://robotics.stackexchange.com/questions/15464/why-do-we-need-a-marginalization-in-bundle-adjustment
                //  https://github.com/jingpang/LearnVIORB/issues/30
                //  https://github.com/RainerKuemmerle/g2o/issues/109
                //  https://zhuanlan.zhihu.com/p/37843131 (in chinese)
                vtx->setMarginalized(true);

                // Register in database
                vtx_container_[id] = vtx;

                // Update max ID
                if (max_vtx_id_ < vtx_id)
                {
                    max_vtx_id_ = vtx_id;
                }

                // Return the created vertex
                return vtx;
            }

        } // namespace g2o
    }     // namespace optimize
} // namespace PLSLAM
