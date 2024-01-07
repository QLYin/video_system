#ifndef YUVOPENGLWIDGET_H
#define YUVOPENGLWIDGET_H

#include <QFile>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class YuvWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit YuvWidget(QWidget *parent = 0);
    ~YuvWidget();

public slots:
    //设置是否yuyv422格式
    void setYuyv(bool yuyv);
    //清空数据
    void clear();
    //设置图片尺寸
    void setFrameSize(int width, int height);
    //更新纹理数据
    void updateTextures(quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 linesizeY, quint32 linesizeU, quint32 linesizeV);
    //统一一个函数
    void updateFrame(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 linesizeY, quint32 linesizeU, quint32 linesizeV);

protected:
    void initializeGL();
    void paintGL();

private:
    void initData();
    void initColor();
    void initShader();
    void initTextures();
    void initParamete();
    void deleteTextures();

private:
    //是否是yuyv422格式
    bool yuyv;
    //图片宽度高度
    int width, height;
    //YUV原数据
    quint8 *dataY, *dataU, *dataV;
    //YUV数据尺寸
    quint32 linesizeY, linesizeU, linesizeV;
    //顶点着色器代码+片段着色器代码
    QString shaderVert, shaderFrag;

    //着色器程序,编译链接着色器
    QOpenGLShaderProgram program;
    //YUV纹理,用于生成纹理贴图
    GLuint textureY, textureU, textureV;
    //shader中YUV变量地址
    GLuint textureUniformY, textureUniformU, textureUniformV;

private:
    //直接读取文件
    QFile file;
    //读取文件内容定时器
    QTimer timer;

private slots:
    //读取文件内容
    void read();

public slots:
    //播放本地文件
    void play(const QString &fileName, int frameRate);
    //停止播放
    void stop();

signals:
    //播放文件结束
    void playFinsh();
};

#endif // YUVOPENGLWIDGET_H
