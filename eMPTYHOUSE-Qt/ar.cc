//=============================================================================================================================
//
// Copyright (c) 2015-2017 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
// EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
// and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
//
//=============================================================================================================================

#include "ar.hpp"
#ifdef _MSC_VER
#pragma warning( push, 1 )
#endif
#include <QtGui>
#ifdef _MSC_VER
#pragma warning( pop )
#endif

namespace easyar {

void eMPTYHOUSE::loadFromImage(std::shared_ptr<easyar::ImageTracker> tracker, const std::string& path)
{
    auto target = std::make_shared<easyar::ImageTarget>();
    std::string jstr = "{\n"
        "  \"images\" :\n"
        "  [\n"
        "    {\n"
        "      \"image\" : \"" + path + "\",\n"
        "      \"name\" : \"" + path.substr(0, path.find_first_of(".")) + "\"\n"
        "    }\n"
        "  ]\n"
        "}";
    target->setup(jstr.c_str(), static_cast<int>(easyar::StorageType::Assets) | static_cast<int>(easyar::StorageType::Json), "");
    tracker->loadTarget(target, [this](std::shared_ptr<easyar::Target> target, bool status) {
        if (!message_)
            return;
        message_(status, "load target (" + std::to_string(status) + "): " + target->name() + " (" + std::to_string(target->runtimeID()) + ")");
    });
}

void eMPTYHOUSE::loadFromJsonFile(std::shared_ptr<easyar::ImageTracker> tracker, const std::string& path, const std::string& targetname)
{
    auto target = std::make_shared<easyar::ImageTarget>();
    target->setup(path, static_cast<int>(easyar::StorageType::Assets), targetname);
    tracker->loadTarget(target, [this](std::shared_ptr<easyar::Target> target, bool status) {
        if (!message_)
            return;
        message_(status, "load target (" + std::to_string(status) + "): " + target->name() + " (" + std::to_string(target->runtimeID()) + ")");
    });
}

void eMPTYHOUSE::loadAllFromJsonFile(std::shared_ptr<easyar::ImageTracker> tracker, const std::string& path)
{
    for (auto && target : easyar::ImageTarget::setupAll(path, static_cast<int>(easyar::StorageType::Assets))) {
        tracker->loadTarget(target, [this](std::shared_ptr<easyar::Target> target, bool status) {
            if (!message_)
                return;
            message_(status, "load target (" + std::to_string(status) + "): " + target->name() + " (" + std::to_string(target->runtimeID()) + ")");
        });
    }
}

bool eMPTYHOUSE::init()
{
    camera_ = std::make_shared<CameraDevice>();
    streamer_ = std::make_shared<CameraFrameStreamer>();
    streamer_->attachCamera(camera_);

    tracker_ = std::make_shared<ImageTracker>();
    tracker_->attachStreamer(streamer_);

    bool status = true;
    status &= camera_->open(static_cast<int>(CameraDeviceType::Default));
    camera_->setSize({{1280, 720}});

    loadAllFromJsonFile(tracker_, "targets.json");
    return status;
}

bool eMPTYHOUSE::start()
{
    if (!camera_)
        return false;
    bool status = true;
    status &= camera_->start();
    status &= streamer_->start();
    camera_->setFocusMode(CameraDeviceFocusMode::Continousauto);

    status &= tracker_->start();
    return status;
}

bool eMPTYHOUSE::stop()
{
    if (!camera_)
        return false;
    bool status = true;
    status &= tracker_->stop();
    status &= streamer_->stop();
    status &= camera_->stop();
    return status;
}

void eMPTYHOUSE::setTrack(bool enable)
{
    if (!tracker_)
        return;
    if (enable)
        tracker_->start();
    else
        tracker_->stop();
}

void eMPTYHOUSE::initGL()
{
    if (renderer_)
        return;
    renderer_ = std::make_shared<Renderer>();
    video_renderer_ = std::make_shared<VideoRenderer>();
    video_renderer_->init();
}

void eMPTYHOUSE::render()
{
    initGL();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Vec4I viewport;
    auto frame = streamer_->peek();
    if (camera_ && camera_->isOpened()) {
        Vec2I p;
        Vec2I size = camera_->size();
        if (view_size_.data[0] / (float)size.data[0] < view_size_.data[1] / (float)size.data[1]) {
            p.data[0] = 0;
            p.data[1] = (int)(view_size_.data[1] - view_size_.data[0] / (float)size.data[0] * (float)size.data[1]) / 2;
        } else {
            p.data[0] = (int)(view_size_.data[0] - view_size_.data[1] / (float)size.data[1] * (float)size.data[0]) / 2;
            p.data[1] = 0;
        }
        viewport = {{p.data[0], p.data[1], view_size_.data[0] - p.data[0] * 2, view_size_.data[1] - p.data[1] * 2}};
    } else if (view_size_.data[0] > 0) {
        viewport = {{0, 0, view_size_.data[0], view_size_.data[1]}};
    }
    glViewport(viewport.data[0], viewport.data[1], viewport.data[2], viewport.data[3]);
    renderer_->render(frame, viewport);

    for (auto && target : frame->targetInstances()) {
        if (target->status() == TargetStatus::Tracked) {
            VideoPlayer* video = new VideoPlayer();
            video->setVideoType(VideoType::Normal);
            video->setRenderTexture((void *)video_renderer_->texId());
            video->open("video.mp4", StorageType::Assets, NULL);
            auto imagetarget = std::dynamic_pointer_cast<ImageTarget>(target->target());
            if (!imagetarget)
                continue;
            video->updateFrame();
            video_renderer_->render(camera_->projectionGL(0.2f, 500.f), target->poseGL(), imagetarget->size());
        }
    }

}

void eMPTYHOUSE::resizeGL(int width, int height)
{
    view_size_ = {width, height};
}

}
