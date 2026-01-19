#ifndef ASI_WRAPPER_H
#define ASI_WRAPPER_H

#ifdef _WIN32
    #define ASI_WRAPPER_API __declspec(dllexport)
#else
    #define ASI_WRAPPER_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Initialize camera and set ROI
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
// Parameters are passed by reference in C++, but for DLL we use pointers
ASI_WRAPPER_API int asi_init_camera(int* cameraID, int* roiWidth, int* roiHeight, int* roiBin, int* imgType);

// Get a frame from the camera
// buffer: pointer to buffer to store frame data
// bufferSize: size of the buffer in bytes
// waitMs: timeout in milliseconds
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_get_frame(int cameraID, unsigned char* buffer, int bufferSize, int waitMs);

// Stop and close the camera
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_stop_camera(int cameraID);

// Start video capture mode
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_start_video_capture(int cameraID);

// Stop video capture mode
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_stop_video_capture(int cameraID);

// Start exposure for snapshot mode
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_start_exposure(int cameraID, int isDark);

// Get exposure status for snapshot mode
// status: pointer to store exposure status
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_get_exp_status(int cameraID, int* status);

// Get data after exposure (snapshot mode)
// buffer: pointer to buffer to store frame data
// bufferSize: size of the buffer in bytes
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_get_data_after_exp(int cameraID, unsigned char* buffer, int bufferSize);

// Set exposure control value
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_set_exposure(int cameraID, long value, int isAuto);

// Set gain control value
// Returns ASI_ERROR_CODE (0 = ASI_SUCCESS)
ASI_WRAPPER_API int asi_set_gain(int cameraID, long value, int isAuto);

#ifdef __cplusplus
}
#endif

#endif // ASI_WRAPPER_H
