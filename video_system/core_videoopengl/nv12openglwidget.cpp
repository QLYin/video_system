﻿#include "nv12openglwidget.h"
#include "openglinclude.h"

Nv12Widget::Nv12Widget(QWidget *parent) : QOpenGLWidget(parent)
{
    QStringList list;
    list << "attribute vec4 vertexIn;";
    list << "attribute vec4 textureIn;";
    list << "varying vec4 textureOut;";

    list << "void main(void)";
    list << "{";
    list << "  gl_Position = vertexIn;";
    list << "  textureOut = textureIn;";
    list << "}";
    shaderVert = list.join("");

    list.clear();
    initFragment(list);
    list << "varying mediump vec4 textureOut;";
    list << "uniform sampler2D textureY;";
    list << "uniform sampler2D textureUV;";
    list << "void main(void)";
    list << "{";
    list << "  vec3 yuv;";
    list << "  vec3 rgb;";
    list << "  yuv.r = texture2D(textureY, textureOut.st).r;";
    list << "  yuv.g = texture2D(textureUV, textureOut.st).r - 0.5;";
    list << "  yuv.b = texture2D(textureUV, textureOut.st).g - 0.5;";
    //list << "  rgb = mat3(1.0, 1.0, 1.0, 0.0, -0.39465, 2.03211, 1.13983, -0.58060, 0.0) * yuv;";
    list << "  rgb = mat3(1.0, 1.0, 1.0, 0.0, -0.3455, 1.779, 1.4075, -0.7169, 0.0) * yuv;";
    list << "  gl_FragColor = vec4(rgb, 1.0);";
    list << "}";
    shaderFrag = list.join("");

    this->initData();

    //关联定时器读取文件
    connect(&timer, SIGNAL(timeout()), this, SLOT(read()));
}

Nv12Widget::~Nv12Widget()
{
    makeCurrent();
    vbo.destroy();
    doneCurrent();
}

void Nv12Widget::clear()
{
    this->initData();
    this->update();
}

void Nv12Widget::setFrameSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Nv12Widget::updateTextures(quint8 *dataY, quint8 *dataUV, quint32 linesizeY, quint32 linesizeUV)
{
    this->dataY = dataY;
    this->dataUV = dataUV;
    this->linesizeY = linesizeY;
    this->linesizeUV = linesizeUV;
    this->update();
}

void Nv12Widget::updateFrame(int width, int height, quint8 *dataY, quint8 *dataUV, quint32 linesizeY, quint32 linesizeUV)
{
    this->setFrameSize(width, height);
    this->updateTextures(dataY, dataUV, linesizeY, linesizeUV);
}

void Nv12Widget::initializeGL()
{
    initializeOpenGLFunctions();
    glDisable(GL_DEPTH_TEST);

    //录制顶点坐标和纹理坐标
    static const GLfloat points[] = {-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

    //顶点缓冲对象初始化
    vbo.create();
    vbo.bind();
    vbo.allocate(points, sizeof(points));

    //初始化shader
    this->initShader();
    //初始化textures
    this->initTextures();
    //初始化颜色
    this->initColor();
}

void Nv12Widget::paintGL()
{
    if (!dataY || !dataUV || width == 0 || height == 0) {
        this->initColor();
        return;
    }

    program.bind();
    program.enableAttributeArray("vertexIn");
    program.enableAttributeArray("textureIn");
    program.setAttributeBuffer("vertexIn", GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
    program.setAttributeBuffer("textureIn", GL_FLOAT, 2 * 4 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureY);
    //字节对齐,网上很多代码都是少了这一步,导致有时候花屏
    glPixelStorei(GL_UNPACK_ROW_LENGTH, linesizeY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, dataY);
    this->initParamete();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureUV);
    //字节对齐,网上很多代码都是少了这一步,导致有时候花屏
    glPixelStorei(GL_UNPACK_ROW_LENGTH, linesizeUV >> 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width >> 1, height >> 1, 0, GL_RG, GL_UNSIGNED_BYTE, dataUV);
    this->initParamete();

    glDrawArrays(GL_QUADS, 0, 4);
    program.setUniformValue("textureY", 1);
    program.setUniformValue("textureUV", 0);
    program.disableAttributeArray("vertexIn");
    program.disableAttributeArray("textureIn");
    program.release();
}

void Nv12Widget::initData()
{
    width = height = 0;
    dataY = dataUV = 0;
    linesizeY = linesizeUV = 0;
}

void Nv12Widget::initColor()
{
    //取画板背景颜色
    QColor color = palette().window().color();
    //设置背景清理色
    glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    //清理颜色背景
    glClear(GL_COLOR_BUFFER_BIT);
}

void Nv12Widget::initShader()
{
    program.addShaderFromSourceCode(QOpenGLShader::Vertex, shaderVert);
    program.addShaderFromSourceCode(QOpenGLShader::Fragment, shaderFrag);
    program.link();
    program.bind();
}

void Nv12Widget::initTextures()
{
    glGenTextures(1, &textureY);
    glGenTextures(1, &textureUV);
}

void Nv12Widget::initParamete()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Nv12Widget::deleteTextures()
{
    glDeleteTextures(1, &textureY);
    glDeleteTextures(1, &textureUV);
}

void Nv12Widget::read()
{
    qint64 len = (width * height * 3) >> 1;
    if (file.read((char *)dataY, len)) {
        this->update();
    } else {
        timer.stop();
        emit playFinsh();
    }
}

void Nv12Widget::play(const QString &fileName, int frameRate)
{
    //停止定时器并关闭文件
    if (timer.isActive()) {
        timer.stop();
    }
    if (file.isOpen()){
        file.close();
    }

    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    //初始化对应数据指针位置
    dataY = new quint8[(width * height * 3) >> 1];
    dataUV = dataY + (width * height);

    //启动定时器读取文件数据
    timer.start(1000 / frameRate);
}

void Nv12Widget::stop()
{
    //停止定时器并关闭文件
    if (timer.isActive()) {
        timer.stop();
    }
    if (file.isOpen()){
        file.close();
    }

    this->clear();
    emit playFinsh();
}
