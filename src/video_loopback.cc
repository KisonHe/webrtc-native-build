/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "video/video_loopback.h"

#include <stdio.h>

#include <memory>
#include <string>
#include <vector>

#include "api/test/simulated_network.h"
#include "api/transport/bitrate_settings.h"
#include "api/video_codecs/video_codec.h"
#include "rtc_base/checks.h"
#include "system_wrappers/include/field_trial.h"
#include "test/field_trial.h"
#include "test/gtest.h"
#include "test/run_test.h"
#include "video/video_quality_test.h"


namespace webrtc {
namespace {

size_t Width() {
  return static_cast<size_t>(640);
}

size_t Height() {
  return static_cast<size_t>(480);
}

int Fps() {
  return 30;
}

size_t GetCaptureDevice() {
  return 0;
}

int MinBitrateKbps() {
  return 50;
}

int StartBitrateKbps() {
  return 300;
}

int TargetBitrateKbps() {
  return 800;
}

int MaxBitrateKbps() {
  return 800;
}

int NumTemporalLayers() {
  return 1;
}

InterLayerPredMode InterLayerPred() {
  // if (absl::GetFlag(FLAGS_inter_layer_pred) == 0) {
    return InterLayerPredMode::kOn;
  // } else if (absl::GetFlag(FLAGS_inter_layer_pred) == 1) {
    // return InterLayerPredMode::kOff;
  // } else {
    // RTC_DCHECK_EQ(absl::GetFlag(FLAGS_inter_layer_pred), 2);
    // return InterLayerPredMode::kOnKeyPic;
// }
}

std::string Codec() {
  return "VP8";
}

int SelectedTL() {
  return -1;
}

int DurationSecs() {
  return 0;
}

std::string OutputFilename() {
  return "";
}

std::string GraphTitle() {
  return "";
}

int LossPercent() {
  return 0;
}

int AvgBurstLossLength() {
  return static_cast<int>(-1);
}

int LinkCapacityKbps() {
  return static_cast<int>(0);
}

int QueueSize() {
  return static_cast<int>(0);
}

int AvgPropagationDelayMs() {
  return static_cast<int>(0);
}

std::string RtcEventLogName() {
  return "";
}

std::string RtpDumpName() {
  return "";
}

int StdPropagationDelayMs() {
  return 0;
}

int NumStreams() {
  return 0;
}

int SelectedStream() {
  return 0;
}

int NumSpatialLayers() {
  return 1;
}

int SelectedSL() {
  return -1;
}

std::string Stream0() {
  return "";
}

std::string Stream1() {
  return "";
}

std::string SL0() {
  return "";
}

std::string SL1() {
  return "";
}

std::string SL2() {
  return "";
}

std::string EncodedFramePath() {
  return "";
}

std::string Clip() {
  return "";
}

}  // namespace

void Loopback() {
  BuiltInNetworkBehaviorConfig pipe_config;
  pipe_config.loss_percent = LossPercent();
  pipe_config.avg_burst_loss_length = AvgBurstLossLength();
  pipe_config.link_capacity_kbps = LinkCapacityKbps();
  pipe_config.queue_length_packets = QueueSize();
  pipe_config.queue_delay_ms = AvgPropagationDelayMs();
  pipe_config.delay_standard_deviation_ms = StdPropagationDelayMs();
  pipe_config.allow_reordering = false;

  BitrateConstraints call_bitrate_config;
  call_bitrate_config.min_bitrate_bps = MinBitrateKbps() * 1000;
  call_bitrate_config.start_bitrate_bps = StartBitrateKbps() * 1000;
  call_bitrate_config.max_bitrate_bps = -1;  // Don't cap bandwidth estimate.

  VideoQualityTest::Params params;
  params.call.send_side_bwe = true;
  params.call.generic_descriptor = false;
  params.call.call_bitrate_config = call_bitrate_config;

  params.video[0].enabled = true;
  params.video[0].width = Width();
  params.video[0].height = Height();
  params.video[0].fps = Fps();
  params.video[0].min_bitrate_bps = MinBitrateKbps() * 1000;
  params.video[0].target_bitrate_bps = TargetBitrateKbps() * 1000;
  params.video[0].max_bitrate_bps = MaxBitrateKbps() * 1000;
  params.video[0].suspend_below_min_bitrate = false;
  params.video[0].codec = Codec();
  params.video[0].num_temporal_layers = NumTemporalLayers();
  params.video[0].selected_tl = SelectedTL();
  params.video[0].min_transmit_bps = 0;
  params.video[0].ulpfec = false;
  params.video[0].flexfec = false;
  params.video[0].automatic_scaling = NumStreams() < 2;
  params.video[0].clip_path = Clip();
  params.video[0].capture_device_index = GetCaptureDevice();
  params.audio.enabled = false;
  params.audio.sync_video = false;
  params.audio.dtx = false;
  params.audio.use_real_adm = false;
  params.logging.rtc_event_log_name = RtcEventLogName();
  params.logging.rtp_dump_name = RtpDumpName();
  params.logging.encoded_frame_base_path = EncodedFramePath();
  params.screenshare[0].enabled = false;
  params.analyzer.test_label = "video";
  params.analyzer.test_durations_secs = DurationSecs();
  params.analyzer.graph_data_output_filename = OutputFilename();
  params.analyzer.graph_title = GraphTitle();
  params.config = pipe_config;

  if (NumStreams() > 1 && Stream0().empty() && Stream1().empty()) {
    params.ss[0].infer_streams = true;
  }

  std::vector<std::string> stream_descriptors;
  stream_descriptors.push_back(Stream0());
  stream_descriptors.push_back(Stream1());
  std::vector<std::string> SL_descriptors;
  SL_descriptors.push_back(SL0());
  SL_descriptors.push_back(SL1());
  SL_descriptors.push_back(SL2());
  VideoQualityTest::FillScalabilitySettings(
      &params, 0, stream_descriptors, NumStreams(), SelectedStream(),
      NumSpatialLayers(), SelectedSL(), InterLayerPred(), SL_descriptors);

  auto fixture = std::make_unique<VideoQualityTest>(nullptr);
  if (DurationSecs()) {
    fixture->RunWithAnalyzer(params);
  } else {
    fixture->RunWithRenderers(params);
  }
}

int RunLoopbackTest(int argc, char* argv[]) {
  // webrtc::field_trial::InitFieldTrialsFromString("");

  // webrtc::test::RunTest(webrtc::Loopback);
  while (1)
  {
    webrtc::Loopback();
  }
  
  return 0;
}
}  // namespace webrtc
