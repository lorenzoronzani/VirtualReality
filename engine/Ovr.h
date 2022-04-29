/**
 * @file		ovr.h
 * @brief	Self-contained helper class for interfacing OpenGL and OpenVR. Shortened version of Overvision's OvVR module.
 *
 * @author	Achille Peternier (C) SUPSI [achille.peternier@supsi.ch]
 */
#pragma once



 //////////////
 // #INCLUDE //
 //////////////
// C/C++:
#include <iostream>
#include <vector>   
// GLM:
#include <glm/gtc/packing.hpp>
// OpenVR:
#include "../dependencies/openvr/include/openvr.h"

////////////////
// CLASS OvVR //
////////////////
/**
 * @brief OpenGL-OpenVR interface.
 */
class OvVR
{
    //////////
public: //
//////////	   

    struct Controller;

    /**
     * Eye enums
     */
    enum OvEye
    {
        EYE_LEFT = 0,
        EYE_RIGHT = 1,

        // Terminator:
        EYE_LAST
    };


    /**
    * Constructor
    */
    OvVR();


    /**
     * Destructor
     */
    ~OvVR();


    /**
     * @brief Init VR components.
     * @return TF
     */
    bool init();


    /**
     * Release VR components.
     * @return TF
     */
    bool free();


    /**
     * Get tracking system name.
     * @return tracking system name
     */
    std::string getTrackingSysName();


    /**
     * Print render models to the screen.
     */
    bool printRenderModels();


    /**
     * Get manufacturer name.
     * @return manufacturer system name
     */
    std::string getManufacturerName();


    /**
     * Get model number (name).
     * @return model number (name)
     */
    std::string getModelNumber();


    /**
     * Get HMD proper horizontal resolution in pixels.
     * @return HMD horizontal resolution in pixels
     */
    unsigned int getHmdIdealHorizRes();


    /**
     * Get HMD proper vertical resolution in pixels.
     * @return HMD vertical resolution in pixels
     */
    unsigned int getHmdIdealVertRes();


    /**
     * Converts an OpenVR 4x3 matrix into an OpenGL one.
     * @param matrix OpenVR 34 matrix
     * @return OpenGL-friendly matrix
     */
    static glm::mat4 ovr2ogl(const vr::HmdMatrix34_t& matrix);


    /**
     * Converts an OpenVR 4x4 matrix into an OpenGL one.
     * @param matrix OpenVR 44 matrix
     * @return Overvision-friendly matrix
     */
    static glm::mat4 ovr2ogl(const vr::HmdMatrix44_t& matrix);


    /**
     * Update poses and internal params. Invoke that once per frame.
     * @return TF
     */
    bool update();


    /**
     * Get the projection matrix for the given eye and plane params.
     * @param eye left or right eye (use enum)
     * @param nearPlane user camera near plane distance
     * @param farPlane user camera far plane distance
     * @return projection matrix ready for OpenGL
     */
    glm::mat4 getProjMatrix(OvEye eye, float nearPlane, float farPlane);


    /**
     * Get the eye-to-head modelview matrix for the given eye.
     * @param eye left or right eye (use enum)
     * @return eye-to-head modelview matrix ready for OpenGL
     */
    glm::mat4 getEye2HeadMatrix(OvEye eye);


    /**
     * Get the user's head modelview position.
     * @return modelview matrix ready for OpenGL
     */
    glm::mat4 getModelviewMatrix();


    /**
     * Get the number of identified and initialized controllers.
     * @return number of controllers
     */
    unsigned int getNrOfControllers();


    /**
     * Get pointer to internal controller reference.
     * @param pos controller position in the list
     * @return pointer or nullptr if error
     */
    Controller* getController(unsigned int pos) const;


    /**
     * Enable/disable reprojection
     * @param flag true or false
     */
    void setReprojection(bool flag);


    /**
     * Pass the left and right textures to the HMD.
     * @param eye left or right eye (use enum)
     * @param eyeTexture OpenGL texture handle
     */
    void pass(OvEye eye, unsigned int eyeTexture);


    /**
     * Once passed the left and right textures, invoke this method to terminate rendering.
     */
    void render();


    // Controller data:
    struct Controller
    {
        unsigned int id;
        vr::VRControllerState_t pControllerState;
        glm::mat4 matrix;

        // Const/dest:
        Controller();
        ~Controller();


        /**
         * Get controller's position matrix.
         * @param id controller id
         * @return model matrix ready for OpenGL
         */
        glm::mat4 getMatrix();


        /**
         * Get controller's axis position.
         * @param axis axis id, between 0 and 4
         * @param bPressed inout, button pressed (use masks to determine which ones)
         * @param bTouched inout, button touched (use masks to determine which ones)
         * @return axis values
         */
        glm::vec2 getAxis(unsigned int axisId, unsigned long long int& bPressed, unsigned long long int& bTouched);


        /**
         * Returns true when the Nth button is enabled (given the input mask).
         * @param id button id
         * @param mask button mask
         * @return true when pressed, false otherwise
         */
        bool isButtonPressed(unsigned long long int id, unsigned long long int mask);
    };


    ///////////
private: //
///////////	

   // OVR objects:	
    vr::IVRSystem* vrSys;
    vr::IVRRenderModels* vrModels;
    vr::IVRCompositor* vrComp;
    vr::TrackedDevicePose_t vrPoses[vr::k_unMaxTrackedDeviceCount];
    std::vector<Controller*> controllers;
};