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

#ifndef PLSLAM_MODULE_LOOP_DETECTOR_H
#define PLSLAM_MODULE_LOOP_DETECTOR_H

#include "core/data/bow_vocabulary.h"
#include "core/module/type.h"
#include "core/optimize/transform_optimizer.h"

#include <atomic>

namespace PLSLAM
{

       namespace data
       {
              class keyframe;
              class bow_database;
       } // namespace data

       namespace module
       {

              class loop_detector
              {
              public:
                     EIGEN_MAKE_ALIGNED_OPERATOR_NEW

                     /**
                      * Constructor
                      */
                     loop_detector(data::bow_database *bow_db, data::bow_vocabulary *bow_vocab, const bool fix_scale_in_Sim3_estimation);

                     /**
                      * Enable loop detection
                      */
                     void enable_loop_detector();

                     /**
                      * Disable loop detection
                      */
                     void disable_loop_detector();

                     /**
                      * Get the loop detector status
                      */
                     bool is_enabled() const;

                     /**
                      * Set the current keyframe
                      */
                     void set_current_keyframe(data::keyframe *keyfrm);

                     /**
                      * Detect loop candidates using BoW vocabulary
                      */
                     bool detect_loop_candidates();

                     /**
                      * Validate loop candidates selected in detect_loop_candidate()
                      */
                     bool validate_candidates();

                     /**
                      * Get the selected candidate keyframe after loop detection and validation
                      */
                     data::keyframe *get_selected_candidate_keyframe() const;

                     /**
                      * Get the estimated Sim3 from the world the the current
                      */
                     g2o::Sim3 get_Sim3_world_to_current() const;

                     /**
                      * Get the matches between the keypoint indices of the current keyframe and the landmarks observed in the candidate
                      */
                     std::vector<data::landmark *> current_matched_landmarks_observed_in_candidate() const;

                     /**
                      * Get the matches between the keypoint indices of the current keyframe and the landmarks observed in covisibilities of the candidate
                      */
                     std::vector<data::landmark *> current_matched_landmarks_observed_in_candidate_covisibilities() const;

                     /**
                      * Set the keyframe ID when loop correction is performed
                      */
                     void set_loop_correct_keyframe_id(const unsigned int loop_correct_keyfrm_id);

              private:
                     /**
                      * Compute the minimum score among covisibilities
                      */
                     float compute_min_score_in_covisibilities(data::keyframe *keyfrm) const;

                     /**
                      * Find continuously detected keyframe sets
                      */
                     keyframe_sets find_continuously_detected_keyframe_sets(const keyframe_sets &prev_cont_detected_keyfrm_sets,
                                                                            const std::vector<data::keyframe *> &keyfrms_to_search) const;

                     /**
                      * Select ONE candidate from the candidates via linear and nonlinear Sim3 validation
                      */
                     bool select_loop_candidate_via_Sim3(const std::vector<data::keyframe *> &loop_candidates,
                                                         data::keyframe *&selected_candidate,
                                                         g2o::Sim3 &g2o_Sim3_world_to_curr,
                                                         std::vector<data::landmark *> &curr_match_lms_observed_in_cand) const;

                     //! BoW database
                     data::bow_database *bow_db_;
                     //! BoW vocabulary
                     data::bow_vocabulary *bow_vocab_;

                     //! transform optimizer
                     const optimize::transform_optimizer transform_optimizer_;

                     //! flag which indicates the loop detector is enabled or not
                     std::atomic<bool> loop_detector_is_enabled_{false};

                     //! for stereo/RGBD models, fix scale when estimating Sim3
                     const bool fix_scale_in_Sim3_estimation_;

                     //-----------------------------------------
                     // variables for loop detection and correction

                     //! current keyframe
                     data::keyframe *cur_keyfrm_;
                     //! final loop candidate
                     data::keyframe *selected_candidate_ = nullptr;

                     //! previously detected keyframe sets as loop candidate
                     keyframe_sets cont_detected_keyfrm_sets_;
                     //! loop candidate for validation
                     std::vector<data::keyframe *> loop_candidates_to_validate_;

                     //! matches between the keypoint indices of the current keyframe and the landmarks observed in the candidate
                     std::vector<data::landmark *> curr_match_lms_observed_in_cand_;
                     //! matches between the keypoint indices of the current keyframe and the landmarks observed in covisibilities of the candidate
                     std::vector<data::landmark *> curr_match_lms_observed_in_cand_covis_;

                     //! the Sim3 camera pose of the current keyframe AFTER loop correction (in Mat44_t format)
                     Mat44_t Sim3_world_to_curr_;
                     //! the Sim3 camera pose of the current keyframe AFTER loop correction (in g2o::Sim3 format)
                     g2o::Sim3 g2o_Sim3_world_to_curr_;

                     //! the keyframe ID when the previouls loop correction was performed
                     unsigned int prev_loop_correct_keyfrm_id_ = 0;

                     //! the threshold of the continuity of continuously detected keyframe set
                     static constexpr unsigned int min_continuity_ = 3;
              };

       } // namespace module
} // namespace PLSLAM

#endif // PLSLAM_MODULE_LOOP_DETECTOR_H
