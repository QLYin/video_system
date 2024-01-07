#ifndef RGBOPENGLWIDGET_H
#define RGBOPENGLWIDGET_H

#include <QFile>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class RgbWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit RgbWidget(QWidget *parent = 0);
    ~RgbWidget();

public slots:
    //清空数据
    void clear();
    //设置图片尺寸
    void setFrameSize(int width, int height);
    //更新纹理数据
    void updateTextures(quint8 *dataRGB, int type);
    //统一一个函数
    void updateFrame(int width, int height, quint8 *dataRGB, int type);

protected:
    void initializeGL();
    void paintGL();

private:
    void initData();
    void initColor();
    void initShader();
    void initTextures();
    void initParamete();

private:
    //RGB数据类型 1-rgb888 2-rgba8888
    int type;
    //图片宽度高度
    int width, height;
    //RGB原数据
    quint8 *dataRGB;
    //顶点着色器代码+片段着色器代码
    QString shaderVert, shaderFrag;

    //顶点缓冲对象
    QOpenGLBuffer vbo;
    //着色器程序,编译链接着色器
    QOpenGLShaderProgram program;
    //RGB纹理,用于生成纹理贴图
    GLuint texture;

private:
    //直接读取图片
    QImage image;
    //直接读取文件
    QFile file;
    //读取文件内容定时器
    QTimer timer;

private slots:
    //读取文件内容
    void read();

public slots:
    //显示图片
    void setImage(const QImage &image);
    //播放本地文件
    void play(const QString &fileName, int frameRate);
    //停止播放
    void stop();

signals:
    //播放文件结束
    void playFinsh();
};

#endif // RGBOPENGLWIDGET_H
