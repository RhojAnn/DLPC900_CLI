#include <thread>
#include <ASICamera2.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <commdlg.h>

// TODO: Document functions aka multi-line comments them

ASI_ERROR_CODE is_camera_connected(){
    int numCameras = ASIGetNumOfConnectedCameras();
    std::cout << "ASIGetNumOfConnectedCameras returned: " << numCameras << std::endl;
    if (numCameras <= 0) {
        std::cout << "No ZWO ASI cameras detected." << std::endl;
        return ASI_ERROR_INVALID_INDEX;
    }
    return ASI_SUCCESS;
}

ASI_ERROR_CODE cmd_set_pos(int cameraID, int startX, int startY) {
    ASI_ERROR_CODE res = ASISetStartPos(cameraID, startX, startY);
    std::cout << "ASISetStartPos (" << startX << "," << startY << ") result: " << res << std::endl;
    return res;
}

// Gets current ROI start position
ASI_ERROR_CODE cmd_get_pos(int cameraID, int& startX, int& startY) {
    ASI_ERROR_CODE res = ASIGetStartPos(cameraID, &startX, &startY);
    std::cout << "ASIGetStartPos: X=" << startX << ", Y=" << startY << " (result: " << res << ")" << std::endl;
    return res;
}

// Sets ROI format
ASI_ERROR_CODE cmd_set_ROI(int cameraID, int roiWidth, int roiHeight, int roiBin, ASI_IMG_TYPE imgType) {
    ASI_ERROR_CODE res = ASISetROIFormat(cameraID, roiWidth, roiHeight, roiBin, imgType);
    std::cout << "ASISetROIFormat (" << roiWidth << "x" << roiHeight << ") result: " << res << std::endl;
    return res;
}

// Gets current ROI format
ASI_ERROR_CODE cmd_get_ROI(int cameraID, int& roiWidth, int& roiHeight, int& roiBin, ASI_IMG_TYPE& imgType) {
    ASI_ERROR_CODE res = ASIGetROIFormat(cameraID, &roiWidth, &roiHeight, &roiBin, &imgType);
    std::cout << "ASIGetROIFormat: " << roiWidth << "x" << roiHeight << ", Bin=" << roiBin << ", ImgType=" << imgType << " (result: " << res << ")" << std::endl;
    return res;
}

// Initializes the camera and sets ROI
ASI_ERROR_CODE cmd_init_camera(int& cameraID, int& roiWidth, int& roiHeight, int& roiBin, ASI_IMG_TYPE& imgType) {
    is_camera_connected();
    
    ASI_CAMERA_INFO info;
    ASI_ERROR_CODE propResult = ASIGetCameraProperty(&info, 0);
    std::cout << "ASIGetCameraProperty result: " << propResult << ", CameraID: " << info.CameraID << std::endl;
    cameraID = info.CameraID;

    ASI_ERROR_CODE openResult = ASIOpenCamera(cameraID);
    std::cout << "ASIOpenCamera result: " << openResult << std::endl;
    if (openResult != ASI_SUCCESS) {
        std::cout << "Failed to open camera." << std::endl;
        return openResult;
    }

    ASI_ERROR_CODE initResult = ASIInitCamera(cameraID);
    std::cout << "ASIInitCamera result: " << initResult << std::endl;
    if (initResult != ASI_SUCCESS) {
        std::cout << "Failed to initialize camera." << std::endl;
        return initResult;
    }

    int numControls = 0;
    ASI_ERROR_CODE controlsResult = ASIGetNumOfControls(cameraID, &numControls);
    std::cout << "ASIGetNumOfControls result: " << controlsResult << ", numControls: " << numControls << std::endl;

    ASI_CONTROL_CAPS caps;
    for (int i = 0; i < numControls; ++i) {
        int capsResult = ASIGetControlCaps(cameraID, i, &caps);
        std::cout << "ASIGetControlCaps index " << i << ": result=" << capsResult
                  << ", Name=" << caps.Name << ", Min=" << caps.MinValue << ", Max=" << caps.MaxValue << std::endl;
    }

    int roiResult = cmd_set_ROI(cameraID, roiWidth, roiHeight, roiBin, imgType);
    if (roiResult != ASI_SUCCESS) {
        std::cout << "Failed to set ROI." << std::endl;
        return initResult;
    }

    int centerX = (8288 - roiWidth) / 2;
    int centerY = (5644 - roiHeight) / 2;

    int posResult = cmd_set_pos(cameraID, centerX, centerY);
    if (posResult != ASI_SUCCESS) {
        std::cout << "Failed to set position." << std::endl;
        return initResult;
    }
    return ASI_SUCCESS;
}

// Stops the camera and closes it
ASI_ERROR_CODE cmd_stop_camera(int cameraID) {
    ASI_ERROR_CODE stopResult = ASIStopVideoCapture(cameraID);
    std::cout << "ASIStopVideoCapture result: " << stopResult << std::endl;
    if(stopResult != ASI_SUCCESS) return static_cast<ASI_ERROR_CODE>(stopResult);
        
    ASI_ERROR_CODE closeResult = ASICloseCamera(cameraID);
    std::cout << "ASICloseCamera result: " << closeResult << std::endl;
    return closeResult;
}

// Shows a Save As dialog and returns the selected file path, or empty string if canceled
std::string save_file(const char* defaultName = "snap_image.png") {
    char szFile[MAX_PATH] = {0};
    strncpy(szFile, defaultName, MAX_PATH - 1);
    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "PNG Files\0*.png\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if (GetSaveFileNameA(&ofn)) {
        return std::string(szFile);
    } else {
        return std::string();
    }
}

// Handles video mode: gets and displays video frames
void cmd_video_mode(int cameraID, cv::Mat& frame, int roiWidth, int roiHeight) {
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
void cmd_snap_mode(int cameraID, cv::Mat& frame, int roiWidth, int roiHeight) {
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
            // Use Windows Save As dialog
            std::string savePath = save_file("snap_image.png");
            if (savePath.empty()) {
                std::cout << "Save dialog canceled. Using default: snap_image.png" << std::endl;
                savePath = "snap_image.png";
            }
            if (cv::imwrite(savePath, frame)) {
                std::cout << "Image saved to: " << savePath << std::endl;
            } else {
                std::cout << "Failed to save image" << std::endl;
            }
        } else {
            std::cout << "Failed to get snap image" << std::endl;
        }
    } else {
        std::cout << "Exposure failed or cancelled" << std::endl;
    }
}


/**
 * Notes：(1) when setting to auto adjust(bAuto=ASI_TRUE)，the lValue should be the current value 
(2) Automatic Exposure and Automatic Gain are only effective in Video mode (that is, when you get 
the image by calling ASIGetVideoData), but not in Snap mode (that is, when you get the image by 
using ASIGetDataAfterExp) 
 */

 /*
return:
ASI_SUCCESS : Operation is successful
ASI_ERROR_CAMERA_CLOSED : camera didn't open
ASI_ERROR_INVALID_ID  :no camera of this ID is connected or ID value is out of boundary
ASI_ERROR_INVALID_CONTROL_TYPE, //invalid Control type
ASI_ERROR_GENERAL_ERROR,//general error, eg: value is out of valid range; operate to camera hareware failed
*/

// Sets exposure controls
ASI_ERROR_CODE cmd_set_exposure(int cameraID, long lValue, ASI_BOOL bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_EXPOSURE, lValue, bAuto);
    std::cout << "ASISetControlValue (EXPOSURE) result: " << res << std::endl;
    return res;
}

// Sets gain controls
ASI_ERROR_CODE cmd_set_gain(int cameraID, long lValue, ASI_BOOL bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_GAIN, lValue, bAuto);
    std::cout << "ASISetControlValue (GAIN) result: " << res << std::endl;
    return res;
}

// Sets offset controls
ASI_ERROR_CODE cmd_set_offset(int cameraID, long lValue, ASI_BOOL bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_OFFSET, lValue, bAuto);
    std::cout << "ASISetControlValue (OFFSET) result: " << res << std::endl;
    return res;
}

// Gets current exposure value
ASI_ERROR_CODE cmd_get_exposure(int cameraID, long& lValue, ASI_BOOL& bAuto) {
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_EXPOSURE, &lValue, &bAuto);
    std::cout << "ASIGetControlValue (EXPOSURE): Value=" << lValue << ", Auto=" << bAuto << " (result: " << res << ")" << std::endl;
    return res;
}

// Gets current gain value
ASI_ERROR_CODE cmd_get_gain(int cameraID, long& lValue, ASI_BOOL& bAuto) {
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_GAIN, &lValue, &bAuto);
    std::cout << "ASIGetControlValue (GAIN): Value=" << lValue << ", Auto=" << bAuto << " (result: " << res << ")" << std::endl;
    return res;
}

// Gets current offset value
ASI_ERROR_CODE cmd_get_offset(int cameraID, long& lValue, ASI_BOOL& bAuto) {
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_OFFSET, &lValue, &bAuto);
    std::cout << "ASIGetControlValue (OFFSET): Value=" << lValue << ", Auto=" << bAuto << " (result: " << res << ")" << std::endl;
    return res;
}


int main() {
    std::cout << "asi_live_view: Starting program." << std::endl;

    int cameraID = 0;
    int roiWidth = 640;
    int roiHeight = 480;
    int roiBin = 1;
    ASI_IMG_TYPE imgType = ASI_IMG_Y8;

    if (cmd_init_camera(cameraID, roiWidth, roiHeight, roiBin, imgType) != ASI_SUCCESS) {
        std::cout << "Camera initialisation failed." << std::endl;
        return 1;
    }


    if(cmd_set_exposure(cameraID, 100, ASI_FALSE) != ASI_SUCCESS) {
        cmd_stop_camera(cameraID);
        return 1;
    }

    if(cmd_set_gain(cameraID, 200, ASI_FALSE) != ASI_SUCCESS) {
        cmd_stop_camera(cameraID);
        return 1;
    }

    cv::Mat frame(roiHeight, roiWidth, CV_8UC1);
    std::string modeChoice;
    std::cout << "Choose mode: [v]ideo or [s]nap? ";
    std::getline(std::cin, modeChoice);
    if (modeChoice == "s" || modeChoice == "S") {
        cmd_snap_mode(cameraID, frame, roiWidth, roiHeight);
    } else {
        ASIStartVideoCapture(cameraID);
        while (true) {
            cmd_video_mode(cameraID, frame, roiWidth, roiHeight);
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