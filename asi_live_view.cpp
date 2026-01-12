#include <ASICamera2.h>
#include <opencv2/opencv.hpp>
#include <iostream>

// Initializes the camera and sets ROI
bool cmd_init_camera(int& cameraID, int& roiWidth, int& roiHeight) {
    int numCameras = ASIGetNumOfConnectedCameras();
    std::cout << "ASIGetNumOfConnectedCameras returned: " << numCameras << std::endl;
    if (numCameras <= 0) {
        std::cout << "No ZWO ASI cameras detected." << std::endl;
        return false;
    }

    ASI_CAMERA_INFO info;
    int propResult = ASIGetCameraProperty(&info, 0);
    std::cout << "ASIGetCameraProperty result: " << propResult << ", CameraID: " << info.CameraID << std::endl;
    cameraID = info.CameraID;

    int openResult = ASIOpenCamera(cameraID);
    std::cout << "ASIOpenCamera result: " << openResult << std::endl;
    if (openResult != ASI_SUCCESS) {
        std::cout << "Failed to open camera." << std::endl;
        return false;
    }

    int initResult = ASIInitCamera(cameraID);
    std::cout << "ASIInitCamera result: " << initResult << std::endl;
    if (initResult != ASI_SUCCESS) {
        std::cout << "Failed to initialize camera." << std::endl;
        return false;
    }

    int numControls = 0;
    int controlsResult = ASIGetNumOfControls(cameraID, &numControls);
    std::cout << "ASIGetNumOfControls result: " << controlsResult << ", numControls: " << numControls << std::endl;

    ASI_CONTROL_CAPS caps;
    for (int i = 0; i < numControls; ++i) {
        int capsResult = ASIGetControlCaps(cameraID, i, &caps);
        std::cout << "ASIGetControlCaps index " << i << ": result=" << capsResult
                  << ", Name=" << caps.Name << ", Min=" << caps.MinValue << ", Max=" << caps.MaxValue << std::endl;
    }

    int roiBin = 1;
    int roiResult = ASISetROIFormat(cameraID, roiWidth, roiHeight, roiBin, ASI_IMG_RAW8);
    std::cout << "ASISetROIFormat (" << roiWidth << "x" << roiHeight << ") result: " << roiResult << std::endl;
    return true;
}

// Stops the camera and closes it
bool cmd_stop_camera(int cameraID) {
    int stopResult = ASIStopVideoCapture(cameraID);
    std::cout << "ASIStopVideoCapture result: " << stopResult << std::endl;
    int closeResult = ASICloseCamera(cameraID);
    std::cout << "ASICloseCamera result: " << closeResult << std::endl;
    return (stopResult == ASI_SUCCESS && closeResult == ASI_SUCCESS);
}

int main() {
    std::cout << "asi_live_view: Starting program." << std::endl;

    int cameraID = 0;
    int roiWidth = 640;
    int roiHeight = 480;
    if (!cmd_init_camera(cameraID, roiWidth, roiHeight)) {
        std::cout << "Camera initialization failed." << std::endl;
        return 1;
    }

    // Set ROI start position
    int startPosResult = ASISetStartPos(cameraID, 0, 0);
    std::cout << "ASISetStartPos result: " << startPosResult << std::endl;


    int expResult = ASISetControlValue(cameraID, ASI_EXPOSURE, 10000, ASI_FALSE);
    std::cout << "ASISetControlValue (EXPOSURE) result: " << expResult << std::endl;
    int gainResult = ASISetControlValue(cameraID, ASI_GAIN, 100, ASI_FALSE);
    std::cout << "ASISetControlValue (GAIN) result: " << gainResult << std::endl;

    cv::Mat frame(roiHeight, roiWidth, CV_8UC1);
    bool videoMode = true;
    bool running = true;
    std::cout << "Press 'v' for video mode, 's' for snap mode, ESC to exit." << std::endl;

    /*
    int startResult = ASIStartVideoCapture(cameraID);
    std::cout << "ASIStartVideoCapture result: " << startResult << std::endl;

    while (running) {
        if (videoMode) {
            int res = ASIGetVideoData(cameraID, frame.data, roiWidth * roiHeight, 1000);
            if (res == ASI_SUCCESS) {
                cv::imshow("ASI Camera Live", frame);
            } else {
                std::cout << "Failed to get video data." << std::endl;
            }
        } else {
            // Snap mode: stop video, start exposure, poll status, get image
            ASIStopVideoCapture(cameraID);
            int startExpRes = ASIStartExposure(cameraID);
            std::cout << "ASIStartExposure result: " << startExpRes << std::endl;
            ASI_EXPOSURE_STATUS status;
            int pollCount = 0;
            do {
                ASIGetExpStatus(cameraID, &status);
                cv::waitKey(10); // Small delay
                ++pollCount;
                if (pollCount > 500) {
                    std::cout << "Exposure timeout." << std::endl;
                    ASIStopExposure(cameraID);
                    break;
                }
            } while (status == ASI_EXP_WORKING);
            if (status == ASI_EXP_SUCCESS) {
                int getDataRes = ASIGetDataAfterExp(cameraID, frame.data, roiWidth * roiHeight);
                std::cout << "ASIGetDataAfterExp result: " << getDataRes << std::endl;
                if (getDataRes == ASI_SUCCESS) {
                    cv::imshow("ASI Camera Live", frame);
                } else {
                    std::cout << "Failed to get snap image." << std::endl;
                }
            } else {
                std::cout << "Exposure failed or cancelled." << std::endl;
            }
        }
        int key = cv::waitKey(1);
        if (key == 27 || cv::getWindowProperty("ASI Camera Live", cv::WND_PROP_VISIBLE) < 1) {
            running = false;
        } else if (key == 'v' || key == 'V') {
            if (!videoMode) {
                int startRes = ASIStartVideoCapture(cameraID);
                std::cout << "Switched to video mode. ASIStartVideoCapture result: " << startRes << std::endl;
                videoMode = true;
            }
        } else if (key == 's' || key == 'S') {
            if (videoMode) {
                ASIStopVideoCapture(cameraID);
                std::cout << "Switched to snap mode." << std::endl;
                videoMode = false;
            } else {
                // In snap mode, start a new exposure on each 's' key press
                int startExpRes = ASIStartExposure(cameraID);
                std::cout << "Snap mode: ASIStartExposure result: " << startExpRes << std::endl;
                ASI_EXPOSURE_STATUS status;
                int pollCount = 0;
                do {
                    ASIGetExpStatus(cameraID, &status);
                    cv::waitKey(10);
                    ++pollCount;
                    if (pollCount > 500) {
                        std::cout << "Exposure timeout." << std::endl;
                        ASIStopExposure(cameraID);
                        break;
                    }
                } while (status == ASI_EXP_WORKING);
                if (status == ASI_EXP_SUCCESS) {
                    int getDataRes = ASIGetDataAfterExp(cameraID, frame.data, roiWidth * roiHeight);
                    std::cout << "ASIGetDataAfterExp result: " << getDataRes << std::endl;
                    if (getDataRes == ASI_SUCCESS) {
                        cv::imshow("ASI Camera Live", frame);
                    } else {
                        std::cout << "Failed to get snap image." << std::endl;
                    }
                } else {
                    std::cout << "Exposure failed or cancelled." << std::endl;
                }
            }
        }
    }

    cmd_stop_camera(cameraID);
    std::cout << "asi_live_view: Exiting program." << std::endl;
    return 0;
    */
}