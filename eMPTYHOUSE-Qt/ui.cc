//=============================================================================================================================
//
// Copyright (c) 2015-2017 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
// EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
// and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
//
//=============================================================================================================================

#include "ui.hpp"
#ifdef _MSC_VER
#pragma warning( push, 1 )
#endif
#include <QtGui>
#include <QtWidgets>
#include "ui_emptyhouse.h"
#ifdef _MSC_VER
#pragma warning( pop )
#endif
#include "ar.hpp"

Q_DECLARE_METATYPE(std::string);

namespace easyar {

GLView::GLView(QWidget *parent)
    : QOpenGLWidget(parent)
{
    timer_ = new QTimer(this);
    timer_->setInterval(16);
    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
}

void GLView::resizeGL(int width, int height)
{
    emit resize(width * devicePixelRatio(), height * devicePixelRatio());
}

void GLView::paintGL()
{
    emit render();
}

void GLView::refreshView()
{
    timer_->setSingleShot(false);
    timer_->start();
}

eMPTYHOUSEWidget::eMPTYHOUSEWidget(QMainWindow *parent)
    : QWidget(parent)
{
    qRegisterMetaType<std::string>();
    parent->resize(813, 556);
    QWidget* centralWidget = new QWidget(parent);
    QGridLayout* gridLayout = new QGridLayout(centralWidget);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(5, 5, 5, 5);
    parent->setCentralWidget(centralWidget);
    QStatusBar* statusBar = new QStatusBar(parent);
    label_busy_ = new QLabel;
    label_msg_ = new QLabel;
    statusBar->addWidget(label_busy_);
    statusBar->addWidget(label_msg_);
    parent->setStatusBar(statusBar);
    parent->setWindowTitle("EasyAR");

    gridLayout->addWidget(this, 0, 0, 1, 1);
    ui_ = std::make_shared<::Ui::eMPTYHOUSEForm>();
    ui_->setupUi(this);
    setupUi();
}

void eMPTYHOUSEWidget::setupUi()
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(1);

    ui_->frame_center->setLayout(new QGridLayout);

    label_busy_->setText(tr("<font color='gray'>Ready</font>"));
    label_busy_->setMinimumWidth(130);
    label_msg_->setText(tr(""));
    label_msg_->setMinimumWidth(200);

    ui_->splitter->setStretchFactor(0, 8);
    ui_->splitter->setStretchFactor(1, 1);

    glview_ = new GLView(this);
    glview_->setMinimumSize(100, 100);
    ((QGridLayout*)ui_->frame_center->layout())->addWidget(glview_, 0, 0, 1, 1);

    connect(glview_, SIGNAL(resize(int, int)), this, SLOT(resizeGL(int, int)));
    connect(glview_, SIGNAL(render()), this, SLOT(renderGL()));
    connect(this, SIGNAL(message(int, std::string)), this, SLOT(onMessage(int, std::string)));

    glview_->refreshView();
}

void eMPTYHOUSEWidget::onMessage(int level, std::string message)
{
    std::string header = "<font color='green'>";
    std::string tail = "</font>";
    if (level <= 0)
        header = "<font color='red'>";
    auto str = header + message + tail;
    label_msg_->setText(tr(str.c_str()));
}

void eMPTYHOUSEWidget::on_pushButton_start_clicked()
{
    if (ui_->pushButton_start->text() == "&Start") {
        bool ret = start();

        if (ret) {
            ui_->pushButton_start->setText("&Pause");
            label_busy_->setText(tr("<font color='red'>Busy...</font>"));
            label_busy_->repaint();
        }
    } else {
        if (ui_->pushButton_start->text() == "&Pause") {
            pause();

            ui_->pushButton_start->setText("&Resume");
            label_busy_->setText(tr("<font color='gray'>Ready</font>"));
            label_busy_->repaint();
        } else {
            resume();

            ui_->pushButton_start->setText("&Pause");
            label_busy_->setText(tr("<font color='red'>Busy...</font>"));
            label_busy_->repaint();
        }
    }
}

void eMPTYHOUSEWidget::on_pushButton_stop_clicked()
{
    stop();

    ui_->pushButton_start->setText("&Start");
    label_busy_->setText(tr("<font color='gray'>Ready</font>"));
    label_busy_->repaint();
}

void eMPTYHOUSEWidget::on_checkBox_track_stateChanged()
{
    if (!ar_)
        return;
    ar_->setTrack(ui_->checkBox_track->isChecked());
}

void eMPTYHOUSEWidget::on_checkBox_barcode_stateChanged()
{
    if (!ar_)
        return;
    ar_->setQRCode(ui_->checkBox_barcode->isChecked());
}

void eMPTYHOUSEWidget::on_checkBox_hflip_stateChanged()
{
    if (!ar_)
        return;
    ar_->setHorizontalFlip(ui_->checkBox_hflip->isChecked());
}

void eMPTYHOUSEWidget::renderGL()
{
    if (!ar_)
        return;
    ar_->resizeGL(glview_size_.width(), glview_size_.height());
    ar_->render();
}

void eMPTYHOUSEWidget::resizeGL(int width, int height)
{
    glview_size_ = {width, height};
}

bool eMPTYHOUSEWidget::start()
{
    auto key = ui_->plainTextEdit->toPlainText().toStdString();
    if (!Engine::initialize(key)) {
        emit message(-1, "Fail to initialize EasyAR");
        return false;
    }
    ar_ = std::make_shared<eMPTYHOUSE>();
    ar_->setMessageHandler([this](int level, std::string msg) {
        emit message(level, msg);
    });
    ar_->init();
    ar_->setTrack(ui_->checkBox_track->isChecked());
    ar_->setQRCode(ui_->checkBox_barcode->isChecked());
    ar_->setHorizontalFlip(ui_->checkBox_hflip->isChecked());
    resume();
    return true;
}

void eMPTYHOUSEWidget::stop()
{
    ar_ = nullptr;
}

void eMPTYHOUSEWidget::pause()
{
    ar_->stop();
}

void eMPTYHOUSEWidget::resume()
{
    ar_->start();
}

}
