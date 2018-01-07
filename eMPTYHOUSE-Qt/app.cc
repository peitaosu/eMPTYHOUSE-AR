//=============================================================================================================================
//
// Copyright (c) 2015-2017 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
// EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
// and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
//
//=============================================================================================================================

#include "ui.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    easyar::eMPTYHOUSEWidget widget(&w);
    w.show();
    return a.exec();
}
