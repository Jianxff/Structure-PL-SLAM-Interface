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

#include "core/util/converter.h"

namespace PLSLAM
{
    namespace util
    {

        std::vector<cv::Mat> converter::to_desc_vec(const cv::Mat &desc)
        {
            std::vector<cv::Mat> desc_vec(desc.rows);
            for (int i = 0; i < desc.rows; ++i)
            {
                desc_vec.at(i) = desc.row(i);
            }
            return desc_vec;
        }

        g2o::SE3Quat converter::to_g2o_SE3(const Mat44_t &cam_pose)
        {
            const Mat33_t rot = cam_pose.block<3, 3>(0, 0);
            const Vec3_t trans = cam_pose.block<3, 1>(0, 3);
            return g2o::SE3Quat{rot, trans};
        }

        Mat44_t converter::to_eigen_mat(const g2o::SE3Quat &g2o_SE3)
        {
            return g2o_SE3.to_homogeneous_matrix();
        }

        Mat44_t converter::to_eigen_mat(const g2o::Sim3 &g2o_Sim3)
        {
            Mat44_t cam_pose = Mat44_t::Identity();
            cam_pose.block<3, 3>(0, 0) = g2o_Sim3.scale() * g2o_Sim3.rotation().toRotationMatrix();
            cam_pose.block<3, 1>(0, 3) = g2o_Sim3.translation();
            return cam_pose;
        }

        Mat44_t converter::to_eigen_cam_pose(const Mat33_t &rot, const Vec3_t &trans)
        {
            Mat44_t cam_pose = Mat44_t::Identity();
            cam_pose.block<3, 3>(0, 0) = rot;
            cam_pose.block<3, 1>(0, 3) = trans;
            return cam_pose;
        }

        Vec3_t converter::to_angle_axis(const Mat33_t &rot_mat)
        {
            const Eigen::AngleAxisd angle_axis(rot_mat);
            return angle_axis.axis() * angle_axis.angle();
        }

        Mat33_t converter::to_rot_mat(const Vec3_t &angle_axis)
        {
            Eigen::Matrix3d rot_mat;
            const double angle = angle_axis.norm();
            if (angle <= 1e-5)
            {
                rot_mat = Eigen::Matrix3d::Identity();
            }
            else
            {
                const Eigen::Vector3d axis = angle_axis / angle;
                rot_mat = Eigen::AngleAxisd(angle, axis).toRotationMatrix();
            }
            return rot_mat;
        }

        Mat33_t converter::to_skew_symmetric_mat(const Vec3_t &vec)
        {
            Mat33_t skew;
            skew << 0, -vec(2), vec(1),
                vec(2), 0, -vec(0),
                -vec(1), vec(0), 0;
            return skew;
        }

    } // namespace util
} // namespace PLSLAM
