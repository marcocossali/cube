#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "geometryengine.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class GeometryEngine;

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void enable_param();
    int find_device_number(const char*);

    void mousePressEvent(QMouseEvent *e) override;
    //void keyPressEvent(QKeyEvent *event) override;

    void timerEvent(QTimerEvent *e) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void initShaders();
    void initTextures();

private:
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometries;

    QOpenGLTexture *texture;

    QMatrix4x4 projection;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    QQuaternion rotation;

    FILE *file;

    int16_t x_acc;
    int16_t y_acc;
    int16_t z_acc;
    uint8_t dati_acc[16];
    //double f_x_acc;
    //double f_y_acc;
    //double f_z_acc;

    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
    uint8_t dati_gyro[16];

    int16_t x_magn;
    int16_t y_magn;
    int16_t z_magn;
    uint8_t dati_magn[16];

    double alpha;

    double timestamp_first;
    double timestamp_actual;
    double timestamp_result;

    //double pitch_acc;
    //double roll_acc;
    double pitch;
    double roll;
    double yaw_gyro;

    int check;
    int check_timestamp;
    int check_start;

    int fd_acc;
    int fd_gyro;

    int device_acc;
    int device_gyro;
    char str[20];
    char path[100];
    int num_dev;
    int ret;
};

#endif // MAINWIDGET_H
