#include <thread>
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

// Handles video mode: gets and displays video frames
void run_video_mode(int cameraID, cv::Mat& frame, int roiWidth, int roiHeight) {
    // int startRes = ASIStartVideoCapture(cameraID);
    // std::cout << "ASIStartVideoCapture result: " << startRes << std::endl;
    ASI_ERROR_CODE res = ASIGetVideoData(cameraID, frame.data, roiWidth * roiHeight, 1000);
    if (res == ASI_SUCCESS) {
        cv::imshow("ASI Camera Live", frame);
    } else {
        std::cout << "Failed to get video data. Return code: " << res << std::endl;
    }
   
}

// Handles snap mode: starts exposure, waits, displays, and saves single frame
void run_snap_mode(int cameraID, cv::Mat& frame, int roiWidth, int roiHeight) {
    ASIStopVideoCapture(cameraID);
    int startExpRes = ASIStartExposure(cameraID, ASI_FALSE);
    std::cout << "ASIStartExposure result: " << startExpRes << std::endl;
    ASI_EXPOSURE_STATUS status;
    int pollCount = 0;
    do {
        ASIGetExpStatus(cameraID, &status);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ++pollCount;
        if (pollCount > 500) {
            std::cout << "Exposure timeout." << std::endl;
            ASIStopExposure(cameraID);
            return;
        }
    } while (status == ASI_EXP_WORKING);
    if (status == ASI_EXP_SUCCESS) {
        int getDataRes = ASIGetDataAfterExp(cameraID, frame.data, roiWidth * roiHeight);
        std::cout << "ASIGetDataAfterExp result: " << getDataRes << std::endl;
        if (getDataRes == ASI_SUCCESS) {
            cv::imshow("ASI Camera Live", frame);
            // Simple save dialog: prompt in console for path
            std::string savePath;
            std::cout << "Enter file path to save image (or leave blank for snap_image.png): ";
            std::getline(std::cin, savePath);
            if (savePath.empty()) savePath = "snap_image.png";
            if (cv::imwrite(savePath, frame)) {
                std::cout << "Image saved to: " << savePath << std::endl;
            } else {
                std::cout << "Failed to save image!" << std::endl;
            }
        } else {
            std::cout << "Failed to get snap image." << std::endl;
        }
    } else {
        std::cout << "Exposure failed or cancelled." << std::endl;
    }
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
    std::string modeChoice;
    std::cout << "Choose mode: [v]ideo or [s]nap? ";
    std::getline(std::cin, modeChoice);
    if (modeChoice == "s" || modeChoice == "S") {
        run_snap_mode(cameraID, frame, roiWidth, roiHeight);
    } else {
        ASIStartVideoCapture(cameraID);
        while (true) {
            run_video_mode(cameraID, frame, roiWidth, roiHeight);
            int key = cv::waitKey(1);
            if (key == 27 || cv::getWindowProperty("ASI Camera Live", cv::WND_PROP_VISIBLE) < 1) {
                break;
            }
        }
        ASIStopVideoCapture(cameraID);
    }

    cmd_stop_camera(cameraID);
    std::cout << "asi_live_view: Exiting program." << std::endl;
    return 0;
}