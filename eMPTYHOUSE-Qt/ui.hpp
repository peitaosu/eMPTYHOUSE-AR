//=============================================================================================================================
//
// Copyright (c) 2015-2017 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
// EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
// and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
//
//=============================================================================================================================

#pragma once

#include <memory>

#ifdef _MSC_VER
#pragma warning( push, 1 )
#endif
#define NOMINMAX
#include <QtCore>
#include <QWidget>
#include <QOpenGLWidget>
#include <QMainWindow>
#include <QtWidgets/QApplication>
#ifdef _MSC_VER
#pragma warning( pop )
#endif
QT_FORWARD_DECLARE_CLASS(QLabel);

namespace Ui {
class eMPTYHOUSEForm;
}

namespace easyar {

class eMPTYHOUSE;

class GLView : public QOpenGLWidget
{
    Q_OBJECT;
public:
    GLView(QWidget *parent = 0);

    void refreshView();
protected:
    void resizeGL(int width, int height);
    void paintGL();
signals:
    void resize(int, int);
    void render();
private:
    QPointer<QTimer> timer_;
};

class eMPTYHOUSEWidget : public QWidget
{
    Q_OBJECT;
public:
    eMPTYHOUSEWidget(QMainWindow *parent = 0);
signals:
    void message(int, std::string);
private slots:
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();

    void renderGL();
    void resizeGL(int width, int height);
    void onMessage(int level, std::string message);
private:
    void setupUi();
    bool start();
    void stop();
    void pause();
    void resume();
private:
    std::shared_ptr<eMPTYHOUSE> ar_;
    std::shared_ptr<::Ui::eMPTYHOUSEForm> ui_;
    QSize glview_size_;
    QPointer<QLabel> label_busy_;
    QPointer<QLabel> label_msg_;
    QPointer<GLView> glview_;
};

}
