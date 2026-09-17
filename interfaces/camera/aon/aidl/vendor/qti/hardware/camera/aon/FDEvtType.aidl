/*
 * Copyright (c) 2023-2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

package vendor.qti.hardware.camera.aon;

/**
 * The event type supported by FaceDetect and FaceDetectPro.
 * These are bit values for client to assign to the fdEvtTypeMask in FDRegisterInfo during RegisterClient
 * and to check the fdEvtTypeMask in FDEvtInfo when receiving the AONCallbackEvent.
 */
@VintfStability
@Backing(type="int")
enum FDEvtType {
    /**
     * This indicates that face detection was performed and a face is detected.
     */
    FaceDetected = 1,
    /**
     * This indicates that face detection was performed and not a face is detected.
     */
    FaceNotDetected = 2,
    /**
     * This indicates that face detection was performed and an eye gaze is detected.
     */
    GazeDetected = 4,
    /**
     * This indicates that face detection was performed and not an eye gaze is detected.
     */
    GazeNotDetected = 8,
    /**
     * This indicates that face detection was performed and a person is detected.
     */
    PersonDetected = 16,
    /**
     * This indicates that face detection was performed and not an person is detected.
     */
    PersonNotDetected = 32,
}
