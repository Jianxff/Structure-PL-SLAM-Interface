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

#ifndef PLSLAM_FEATURE_ORB_EXTRACTOR_H
#define PLSLAM_FEATURE_ORB_EXTRACTOR_H

#include "core/feature/orb_params.h"
#include "core/feature/orb_extractor_node.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace PLSLAM
{
    namespace feature
    {

        class orb_extractor
        {
        public:
            orb_extractor() = delete;

            //! Constructor
            orb_extractor(const unsigned int max_num_keypts, const float scale_factor, const unsigned int num_levels,
                          const unsigned int ini_fast_thr, const unsigned int min_fast_thr,
                          const std::vector<std::vector<float>> &mask_rects = {});

            //! Constructor
            explicit orb_extractor(const orb_params &orb_params);

            //! Destructor
            virtual ~orb_extractor() = default;

            //! Extract keypoints and each descriptor of them
            void extract(const cv::_InputArray &in_image, const cv::_InputArray &in_image_mask,
                         std::vector<cv::KeyPoint> &keypts, const cv::_OutputArray &out_descriptors);

            //! Get the maximum number of keypoints
            unsigned int get_max_num_keypoints() const;

            //! Set the maximum number of keypoints
            void set_max_num_keypoints(const unsigned int max_num_keypts);

            //! Get the scale factor
            float get_scale_factor() const;

            //! Set the scale factor
            void set_scale_factor(const float scale_factor);

            //! Get the number of scale levels
            unsigned int get_num_scale_levels() const;

            //! Set the number of scale levels
            void set_num_scale_levels(const unsigned int num_levels);

            //! Get the initial fast threshold
            unsigned int get_initial_fast_threshold() const;

            //! Set the initial fast threshold
            void set_initial_fast_threshold(const unsigned int initial_fast_threshold);

            //! Get the minimum fast threshold
            unsigned int get_minimum_fast_threshold() const;

            //! Set the minimum fast threshold
            void set_minimum_fast_threshold(const unsigned int minimum_fast_threshold);

            //! Get scale factors
            std::vector<float> get_scale_factors() const;

            //! Set scale factors
            std::vector<float> get_inv_scale_factors() const;

            //! Get sigma square for all levels
            std::vector<float> get_level_sigma_sq() const;

            //! Get inverted sigma square for all levels
            std::vector<float> get_inv_level_sigma_sq() const;

            //! Image pyramid
            std::vector<cv::Mat> image_pyramid_;

        private:
            //! Initialize orb extractor
            void initialize();

            //! Calculate scale factors and sigmas
            void calc_scale_factors();

            //! Create a mask matrix that constructed by rectangles
            void create_rectangle_mask(const unsigned int cols, const unsigned int rows);

            //! Compute image pyramid
            void compute_image_pyramid(const cv::Mat &image);

            //! Compute fast keypoints for cells in each image pyramid
            void compute_fast_keypoints(std::vector<std::vector<cv::KeyPoint>> &all_keypts, const cv::Mat &mask) const;

            //! Pick computed keypoints on the image uniformly
            std::vector<cv::KeyPoint> distribute_keypoints_via_tree(const std::vector<cv::KeyPoint> &keypts_to_distribute,
                                                                    const int min_x, const int max_x, const int min_y, const int max_y,
                                                                    const unsigned int num_keypts) const;

            //! Initialize nodes that used for keypoint distribution tree
            std::list<orb_extractor_node> initialize_nodes(const std::vector<cv::KeyPoint> &keypts_to_distribute,
                                                           const int min_x, const int max_x, const int min_y, const int max_y) const;

            //! Assign child nodes to the all node list
            void assign_child_nodes(const std::array<orb_extractor_node, 4> &child_nodes, std::list<orb_extractor_node> &nodes,
                                    std::vector<std::pair<int, orb_extractor_node *>> &leaf_nodes) const;

            //! Find keypoint which has maximum value of response
            std::vector<cv::KeyPoint> find_keypoints_with_max_response(std::list<orb_extractor_node> &nodes) const;

            //! Compute orientation for each keypoint
            void compute_orientation(const cv::Mat &image, std::vector<cv::KeyPoint> &keypts) const;

            //! Correct keypoint's position to comply with the scale
            void correct_keypoint_scale(std::vector<cv::KeyPoint> &keypts_at_level, const unsigned int level) const;

            //! Compute the gradient direction of pixel intensity in a circle around the point
            float ic_angle(const cv::Mat &image, const cv::Point2f &point) const;

            //! Compute orb descriptors for all keypoint
            void compute_orb_descriptors(const cv::Mat &image, const std::vector<cv::KeyPoint> &keypts, cv::Mat &descriptors) const;

            //! Compute orb descriptor of a keypoint
            void compute_orb_descriptor(const cv::KeyPoint &keypt, const cv::Mat &image, uchar *desc) const;

            //! parameters for ORB extraction
            orb_params orb_params_;

            //! BRIEF orientation
            static constexpr unsigned int fast_patch_size_ = 31;
            //! half size of FAST patch
            static constexpr int fast_half_patch_size_ = fast_patch_size_ / 2;

            //! size of maximum ORB patch radius
            static constexpr unsigned int orb_patch_radius_ = 19;

            //! rectangle mask has been already initialized or not
            bool mask_is_initialized_ = false;
            cv::Mat rect_mask_;

            //! A list of the scale factor of each pyramid layer
            std::vector<float> scale_factors_;
            std::vector<float> inv_scale_factors_;
            //! A list of the sigma of each pyramid layer
            std::vector<float> level_sigma_sq_;
            std::vector<float> inv_level_sigma_sq_;

            //! Maximum number of keypoint of each level
            std::vector<unsigned int> num_keypts_per_level_;
            //! Index limitation that used for calculating of keypoint orientation
            std::vector<int> u_max_;
        };

    } // namespace feature
} // namespace PLSLAM

#endif // PLSLAM_FEATURE_ORB_EXTRACTOR_H
