#include "qthelperfile.h"
#include "qthelper.h"

void QtHelperFile::initDialog(QFileDialog *dialog, const QString &title, const QString &acceptName,
                           const QString &dirName, bool native, int width, int height)
{
    //设置标题
    dialog->setWindowTitle(title);
    //设置标签文本
    dialog->setLabelText(QFileDialog::Accept, acceptName);
    dialog->setLabelText(QFileDialog::Reject, "取消(&C)");
    dialog->setLabelText(QFileDialog::LookIn, "查看");
    dialog->setLabelText(QFileDialog::FileName, "名称");
    dialog->setLabelText(QFileDialog::FileType, "类型");

    //设置默认显示目录
    if (!dirName.isEmpty()) {
        dialog->setDirectory(dirName);
    }

    //设置对话框宽高
    if (width > 0 && height > 0) {
#ifdef Q_OS_ANDROID
        bool horizontal = (QtHelper::deskWidth() > QtHelper::deskHeight());
        if (horizontal) {
            width = QtHelper::deskWidth() / 2;
            height = QtHelper::deskHeight() - 50;
        } else {
            width = QtHelper::deskWidth() - 10;
            height = QtHelper::deskHeight() / 2;
        }
#endif
        dialog->setFixedSize(width, height);
    }

    //设置是否采用本地对话框
    dialog->setOption(QFileDialog::DontUseNativeDialog, !native);
    //设置只读可以取消右上角的新建按钮
    //dialog->setReadOnly(true);
}

QString QtHelperFile::getDialogResult(QFileDialog *dialog)
{
    QString result;
    if (dialog->exec() == QFileDialog::Accepted) {
        result = dialog->selectedFiles().first();
        if (!result.contains(".")) {
            //自动补全拓展名 保存文件(*.txt *.exe)
            QString filter = dialog->selectedNameFilter();
            if (filter.contains("*.")) {
                filter = filter.split("(").last();
                filter = filter.mid(0, filter.length() - 1);
                //取出第一个作为拓展名
                if (!filter.contains("*.*")) {
                    filter = filter.split(" ").first();
                    result = result + filter.mid(1, filter.length());
                }
            }
        }
    }
    return result;
}

QString QtHelperFile::getOpenFileName(const QString &filter, const QString &dirName, const QString &fileName,
                                   bool native, int width, int height)
{
    QFileDialog dialog;
    initDialog(&dialog, "打开文件", "选择(&S)", dirName, native, width, height);

    //设置文件类型
    if (!filter.isEmpty()) {
        dialog.setNameFilter(filter);
    }

    //设置默认文件名称
    dialog.selectFile(fileName);
    return getDialogResult(&dialog);
}

QString QtHelperFile::getSaveFileName(const QString &filter, const QString &dirName, const QString &fileName,
                                   bool native, int width, int height)
{
    QFileDialog dialog;
    initDialog(&dialog, "保存文件", "保存(&S)", dirName, native, width, height);

    //设置文件类型
    if (!filter.isEmpty()) {
        dialog.setNameFilter(filter);
    }

    //设置默认文件名称
    dialog.selectFile(fileName);
    //设置模态类型允许输入
    dialog.setWindowModality(Qt::WindowModal);
    //设置置顶显示
    dialog.setWindowFlags(dialog.windowFlags() | Qt::WindowStaysOnTopHint);
    return getDialogResult(&dialog);
}

QString QtHelperFile::getExistingDirectory(const QString &dirName, bool native, int width, int height)
{
    QFileDialog dialog;
    initDialog(&dialog, "选择目录", "选择(&S)", dirName, native, width, height);
    dialog.setOption(QFileDialog::ReadOnly);
    //设置只显示目录
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    dialog.setFileMode(QFileDialog::DirectoryOnly);
#endif
    dialog.setOption(QFileDialog::ShowDirsOnly);
    return getDialogResult(&dialog);
}

QString QtHelperFile::getFileNameWithExtension(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    return fileInfo.fileName();
}

QStringList QtHelperFile::getFolderFileNames(const QStringList &filter)
{
    QStringList fileList;
    QString strFolder = QFileDialog::getExistingDirectory();
    if (!strFolder.length() == 0) {
        QDir myFolder(strFolder);
        if (myFolder.exists()) {
            fileList = myFolder.entryList(filter);
        }
    }

    return fileList;
}

bool QtHelperFile::copyFile(const QString &sourceFile, const QString &targetFile)
{
    bool ok;
    ok = QFile::copy(sourceFile, targetFile);
    //将复制过去的文件只读属性取消
    ok = QFile::setPermissions(targetFile, QFile::WriteOwner);
    return ok;
}

void QtHelperFile::deleteDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            fi.dir().remove(fi.fileName());
        } else {
            deleteDirectory(fi.absoluteFilePath());
            dir.rmdir(fi.absoluteFilePath());
        }
    }
}
