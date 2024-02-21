#include "rgbopenglwidget.h"
#include "openglinclude.h"

RgbWidget::RgbWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    QStringList list;
    list << "#version 330 \n";
    list << "attribute vec4 vertexIn;";
    list << "attribute vec2 textureIn;";
    list << "varying vec2 textureOut;";
    list << "void main(void)";
    list << "{";
    list << "  gl_Position = vertexIn;";
    list << "  textureOut = textureIn;";
    list << "}";
    shaderVert = list.join("");

    list.clear();
    list << "#version 330 \n";
    list << "varying vec2 textureOut;";
    list << "uniform sampler2D textureRgb;";
    list << "void main(void)";
    list << "{";
    list << "  gl_FragColor = texture(textureRgb, textureOut);";
    list << "}";
    shaderFrag = list.join("");
}

RgbWidget::~RgbWidget()
{
    makeCurrent();
    vbo.destroy();
    doneCurrent();
}

void RgbWidget::clear()
{
    this->initData();
    this->update();
}

void RgbWidget::setFrameSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void RgbWidget::updateTextures(quint8 *dataRGB, int type)
{
    this->dataRGB = dataRGB;
    this->type = type;
    this->update();
}

void RgbWidget::updateFrame(int width, int height, quint8 *dataRGB, int type)
{
    this->setFrameSize(width, height);
    this->updateTextures(dataRGB, type);
}

void RgbWidget::initializeGL()
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

void RgbWidget::paintGL()
{
    if (!dataRGB || width == 0 || height == 0) {
        this->initColor();
        return;
    }

    program.bind();
    program.enableAttributeArray(0);
    program.enableAttributeArray(1);
    program.setAttributeBuffer(0, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
    program.setAttributeBuffer(1, GL_FLOAT, 2 * 4 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    //1-rgb888 2-rgba8888
    if (type == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataRGB);
    } else if (type == 2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataRGB);
    }

    this->initParamete();

    glDrawArrays(GL_QUADS, 0, 4);
    program.setUniformValue("textureRgb", 0);
    program.disableAttributeArray(0);
    program.disableAttributeArray(1);
    program.release();
}

void RgbWidget::initData()
{
    width = height = 0;
    dataRGB = 0;
}

void RgbWidget::initColor()
{
    //取画板背景颜色
    QColor color = palette().window().color();
    //设置背景清理色
    glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    //清理颜色背景
    glClear(GL_COLOR_BUFFER_BIT);
}

void RgbWidget::initShader()
{
    program.addShaderFromSourceCode(QOpenGLShader::Vertex, shaderVert);
    program.addShaderFromSourceCode(QOpenGLShader::Fragment, shaderFrag);
    program.link();
    program.bind();
}

void RgbWidget::initTextures()
{
    glGenTextures(1, &texture);
}

void RgbWidget::initParamete()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void RgbWidget::read()
{

}

void RgbWidget::setImage(const QImage &image)
{
    QImage img = image;
    if (img.format() != QImage::Format_RGB888) {
        img = img.convertToFormat(QImage::Format_RGB888);
    }

    this->updateFrame(img.width(), img.height(), img.bits(), 1);
}

void RgbWidget::play(const QString &fileName, int frameRate)
{
    image = QImage(fileName);
    if (image.isNull()) {
        return;
    }

    if (image.format() != QImage::Format_RGBA8888) {
        image = image.convertToFormat(QImage::Format_RGBA8888);
    }

    this->updateFrame(image.width(), image.height(), image.bits(), 2);
    emit playFinsh();
}

void RgbWidget::stop()
{

}
