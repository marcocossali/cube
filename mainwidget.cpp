#include "mainwidget.h"
#include <QMouseEvent>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent), geometries(0), texture(0){

    check = 0;
    check_start = 0;
    check_timestamp = 0;
    timestamp_actual = 0.0f;
    timestamp_first = 0.0f;
    timestamp_result = 0.0f;
    alpha = 0.95;
    enable_param();
}

MainWidget::~MainWidget(){

    // Make sure the context is current when deleting the texture and the buffers.
    makeCurrent();
    delete texture;
    delete geometries;
    doneCurrent();
}

// Search the number of device passed like parameter
int MainWidget::find_device_number(const char *sensor_name){

    FILE *file;

    DIR *dr = opendir("/sys/bus/iio/devices");
    struct dirent *in_file;

    if(dr != NULL)
    {
        while((in_file = readdir(dr)) != NULL){
            if(strncmp(in_file->d_name,"iio:device",10) == 0)
                num_dev++;
        }
        closedir(dr);
    }


    for(ret=0; ret<num_dev; ret++){
        sprintf(path, "/sys/bus/iio/devices/iio:device%d/name", ret);
        file = fopen(path, "r");
        if(file != NULL){
            fscanf(file, "%[^\n]", str);
            fclose(file);
        }
        if(strcmp(sensor_name,str) == 0)
            break;
    }

    return ret;
}

// Enable channel for use the buffer, set frequency, buffer length and buffer watermark
void MainWidget::enable_param(){

    device_acc = find_device_number("lsm6dsl_accel");
    device_gyro = find_device_number("lsm6dsl_gyro");

    qDebug() << QString("%1").arg(device_acc) << "," << QString("%1").arg(device_gyro);

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/scan_elements/in_accel_x_en", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/scan_elements/in_accel_y_en", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/scan_elements/in_accel_z_en", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }
    sprintf(path, "/sys/bus/iio/devices/iio:device%d/scan_elements/in_timestamp_en", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/sampling_frequency", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "208");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/buffer/enable", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "0");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/buffer/length", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "25");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/buffer/watermark", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "12");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/buffer/enable", device_acc);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/dev/iio:device%d", device_acc);
    fd_acc = open(path, O_RDONLY | O_NONBLOCK);

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/scan_elements/in_anglvel_x_en", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/scan_elements/in_anglvel_y_en", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/scan_elements/in_anglvel_z_en", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/scan_elements/in_timestamp_en", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/sys/bus/iio/devices/iio:device%d/sampling_frequency", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "208");
        fclose(file);
    }
    sprintf(path, "/sys/bus/iio/devices/iio:device%d/buffer/enable", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "0");
        fclose(file);

    }
    sprintf(path, "/sys/bus/iio/devices/iio:device%d/buffer/length", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "25");
        fclose(file);
    }
    sprintf(path, "/sys/bus/iio/devices/iio:device%d/buffer/watermark", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "12");
        fclose(file);
    }
    sprintf(path, "/sys/bus/iio/devices/iio:device%d/buffer/enable", device_gyro);
    file = fopen(path, "w");
    if(file != NULL){
        fprintf(file,"%s", "1");
        fclose(file);
    }

    sprintf(path, "/dev/iio:device%d", device_gyro);
    fd_gyro = open(path, O_RDONLY | O_NONBLOCK);
}

void MainWidget::mousePressEvent(QMouseEvent *e){

    // Calibration of angles
    if (e->localPos().x() > 240 && e->localPos().x() < 550 && e->localPos().y() > 100 && e->localPos().y() < 370){
        pitch = 0;
        roll = 0;
        yaw_gyro = 0;
        qDebug() << "Calibration OK";
    }
    else
         qDebug() << "Tap the cube";
}

void MainWidget::timerEvent(QTimerEvent *){

    while(1){

        if(read(fd_acc, dati_acc, 16) >1 && read(fd_gyro, dati_gyro, 16) > 1){

            x_acc = *((int16_t*)(dati_acc));
            y_acc = *((int16_t*)(dati_acc+2));
            z_acc = *((int16_t*)(dati_acc+4));

            x_gyro = *((int16_t*)(dati_gyro));
            y_gyro = *((int16_t*)(dati_gyro+2));
            z_gyro = *((int16_t*)(dati_gyro+4));

            if (check_timestamp == 0){
                timestamp_first = *((int64_t*)(dati_gyro+8));
                check_timestamp = 1;
            }
            else{
                timestamp_actual = *((int64_t*)(dati_gyro+8));
                timestamp_result =(timestamp_actual - timestamp_first);
                timestamp_first = timestamp_actual;

                if(x_gyro > 100 || x_gyro < -100 || y_gyro > 100 || y_gyro < -100 || z_gyro > 100 || z_gyro < -100){

                    // Pitch & roll calculated with complementary filter (accelerometer and gyroscope data)
                    pitch = alpha*(pitch + (y_gyro*timestamp_result)*0.000000000009) + (1 - alpha)*(atan2(-x_acc,sqrt(y_acc*y_acc+z_acc*z_acc))*180/M_PI);
                    printf("pitch:%d°, ", (int)pitch);

                    roll = alpha*(roll + (x_gyro*timestamp_result)*0.000000000009) + (1 - alpha)*(atan2(y_acc,sqrt(x_acc*x_acc+z_acc*z_acc))*180/M_PI);
                    printf("roll:%d°, ", (int)roll);

                    // Yaw calculated with gyroscope data
                    yaw_gyro += ((z_gyro*timestamp_result)/100000000000)*0.9;
                    if(yaw_gyro < -360 || yaw_gyro > 360)
                        yaw_gyro = 0;
                    printf("yaw:%d°\n", (int)yaw_gyro);
                }
            }
        }
        else
            break;
    }
    // Update rotation
    rotation = QQuaternion::fromEulerAngles(pitch,-roll,yaw_gyro);
    update();
}

void MainWidget::initializeGL(){

    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    geometries = new GeometryEngine;

    // Start timer event every 12ms
    timer.start(12, this);
}

void MainWidget::initShaders(){

    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void MainWidget::initTextures(){

    // Load cube.png image
    texture = new QOpenGLTexture(QImage(":/cube_face.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::resizeGL(int w, int h){

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL(){

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    geometries->drawCubeGeometry(&program);
}
