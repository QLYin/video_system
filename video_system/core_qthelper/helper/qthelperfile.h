#ifndef QTHELPERFILE_H
#define QTHELPERFILE_H

#include <QObject>
#include <QCoreApplication>
class QFileDialog;

class QtHelperFile
{
public:
    //为什么还要自定义对话框因为可控宽高和汉化对应文本等
    //初始化对话框文本
    static void initDialog(QFileDialog *dialog, const QString &title, const QString &acceptName,
                           const QString &dirName, bool native, int width, int height);
    //拿到对话框结果
    static QString getDialogResult(QFileDialog *dialog);
    //选择文件对话框
    static QString getOpenFileName(const QString &filter = QString(),
                                   const QString &dirName = QString(),
                                   const QString &fileName = QString(),
                                   bool native = false, int width = 900, int height = 600);
    //保存文件对话框
    static QString getSaveFileName(const QString &filter = QString(),
                                   const QString &dirName = QString(),
                                   const QString &fileName = QString(),
                                   bool native = false, int width = 900, int height = 600);
    //选择目录对话框
    static QString getExistingDirectory(const QString &dirName = QString(),
                                        bool native = false, int width = 900, int height = 600);

    //获取文件名,含拓展名
    static QString getFileNameWithExtension(const QString &fileName);
    //获取选择文件夹中的文件
    static QStringList getFolderFileNames(const QStringList &filter);

    //复制文件
    static bool copyFile(const QString &sourceFile, const QString &targetFile);
    //删除文件夹下所有文件
    static void deleteDirectory(const QString &path);
};

#endif // QTHELPERFILE_H
