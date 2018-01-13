#include "arvideo.hpp"

ARVideo::ARVideo()
{
    prepared_ = false;
    found_ = false;
    callback_ = NULL;
}

ARVideo::~ARVideo()
{
    player_.close();
    if(callback_)
        delete callback_;
}

void ARVideo::openVideoFile(const std::string& path, int texid)
{
    if(!callback_)
        callback_ = new CallBack(this);
    path_ = path;
    player_.setRenderTexture((void *)texid);
    player_.setVideoType(easyar::VideoType::Normal);
    player_.open(path.c_str(), easyar::StorageType::Assets, callback_);
}

void ARVideo::openTransparentVideoFile(const std::string& path, int texid)
{
    if(!callback_)
        callback_ = new CallBack(this);
    path_ = path;
    player_.setRenderTexture((void *)texid);
    player_.setVideoType(easyar::VideoType::TransparentSideBySide);
    player_.open(path.c_str(), easyar::StorageType::Assets, callback_);
}

void ARVideo::openStreamingVideo(const std::string& url, int texid)
{
    if(!callback_)
        callback_ = new CallBack(this);
    path_ = url;
    player_.setRenderTexture((void *)texid);
    player_.setVideoType(easyar::VideoType::Normal);
    player_.open(url.c_str(), easyar::StorageType::Assets, callback_);
}

void ARVideo::setVideoStatus(easyar::VideoStatus status)
{
    if (status == easyar::VideoStatus::Ready) {
        prepared_ = true;
        if (found_)
            player_.play();
    }
    if (status == easyar::VideoStatus::Completed) {
        if (found_)
            player_.play();
    }
}

void ARVideo::onFound()
{
    found_ = true;
    if (prepared_) {
        player_.play();
    }
}

void ARVideo::onLost()
{
    found_ = false;
    if (prepared_)
        player_.pause();
}

void ARVideo::update()
{
    player_.updateFrame();
}

ARVideo::CallBack::CallBack(ARVideo* video)
{
    video_ = video;
}

void ARVideo::CallBack::operator() (easyar::VideoStatus status)
{
    video_->setVideoStatus(status);
}
