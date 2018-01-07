//=============================================================================================================================
//
// Copyright (c) 2015-2017 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
// EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
// and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
//
//=============================================================================================================================

#pragma once

#include "easyar/frame.hpp"
#include "easyar/image.hpp"
#include "easyar/matrix.hpp"
#include "easyar/vector.hpp"
#include "easyar/camera.hpp"
#include "easyar/player.hpp"
#include "easyar/renderer.hpp"
#include "easyar/engine.hpp"
#include "easyar/framestreamer.hpp"
#include "easyar/qrcode.hpp"

#include "boxrenderer.hpp"
#include "easyar/imagetarget.hpp"
#include "easyar/imagetracker.hpp"
#include <QString>
#include <QObject>

namespace easyar {

class eMPTYHOUSE
{
public:
    void setMessageHandler(std::function<void(int, std::string)> func) { message_ = func; }

    bool init();
    bool start();
    bool stop();

    void render();
    void initGL();
    void resizeGL(int width, int height);

    void setQRCode(bool enable);
    void setTrack(bool enable);
    void setHorizontalFlip(bool flip);
private:
    void loadFromImage(std::shared_ptr<easyar::ImageTracker> tracker, const std::string& path);
    void loadFromJsonFile(std::shared_ptr<easyar::ImageTracker> tracker, const std::string& path, const std::string& targetname);
    void loadAllFromJsonFile(std::shared_ptr<easyar::ImageTracker> tracker, const std::string& path);
private:
    std::shared_ptr<CameraDevice> camera_;
    std::shared_ptr<CameraFrameStreamer> streamer_;
    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<QRCodeScanner> qrcode_;
    std::shared_ptr<ImageTracker> tracker_;

    std::shared_ptr<BoxRenderer> box_renderer_;
    easyar::Vec2I view_size_;
    bool track_enabled_{true};
    bool qrcode_enabled_{false};
    std::function<void(int, std::string)> message_;
};

}
