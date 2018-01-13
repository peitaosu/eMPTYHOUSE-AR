#include "easyar/player.hpp"

class VideoPlayerCallBack
{
public:
    virtual void operator() (easyar::VideoStatus status) = 0;
};

class ARVideo {

public:
    ARVideo();
    ~ARVideo();

    void openVideoFile(const std::string& path, int texid);
    void openTransparentVideoFile(const std::string& path, int texid);
    void openStreamingVideo(const std::string& url, int texid);

    void setVideoStatus(easyar::VideoStatus status);
    void onFound();
    void onLost();
    void update();

    class CallBack : public VideoPlayerCallBack
    {
    public:
        CallBack(ARVideo* video);
        void operator() (easyar::VideoStatus status);
    private:
        ARVideo* video_;
    };

private:
    easyar::VideoPlayer player_;
    bool prepared_;
    bool found_;
    CallBack* callback_;
    std::string path_;
};
