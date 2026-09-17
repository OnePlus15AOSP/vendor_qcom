/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */
package vendor.qti.hardware.camera.aon;
@VintfStability
parcelable CameraRectangle {

  /**
   * Top left coordinate of the rectangle
   */
  int x;
  int y;

  /**
   * Width & Height of the rectangle
   */
  int width;
  int height;
}
