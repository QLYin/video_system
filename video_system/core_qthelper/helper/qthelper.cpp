#include "qthelper.h"
#include "qthelpercore.h"
#include "qthelperdata.h"
#include "qthelperfile.h"
#include "qthelpernet.h"
#include "qthelperform.h"
#include "qthelperimage.h"
#include "qthelperother.h"

QList<QRect> QtHelper::getScreenRects(bool available)
{
    return QtHelperCore::getScreenRects(available);
}

int QtHelper::getScreenIndex()
{
    return QtHelperCore::getScreenIndex();
}

QRect QtHelper::getScreenRect(bool available)
{
    return QtHelperCore::getScreenRect(available);
}

qreal QtHelper::getScreenRatio(int index, bool devicePixel)
{
    return QtHelperCore::getScreenRatio(index, devicePixel);
}

QRect QtHelper::checkCenterRect(QRect &rect, bool available)
{
    return QtHelperCore::checkCenterRect(rect, available);
}

int QtHelper::deskWidth()
{
    return QtHelperCore::deskWidth();
}

int QtHelper::deskHeight()
{
    return QtHelperCore::deskHeight();
}

QSize QtHelper::deskSize()
{
    return QtHelperCore::deskSize();
}

QWidget *QtHelper::centerBaseForm = 0;
void QtHelper::setFormInCenter(QWidget *form)
{
    QtHelperCore::setFormInCenter(form, centerBaseForm);
}

void QtHelper::showForm(QWidget *form)
{
    QtHelperCore::showForm(form);
}

QString QtHelper::appName()
{
    return QtHelperCore::appName();
}

QString QtHelper::appPath()
{
    return QtHelperCore::appPath();
}

void QtHelper::getCurrentInfo(char *argv[], QString &path, QString &name)
{
    QtHelperCore::getCurrentInfo(argv, path, name);
}

QString QtHelper::getIniValue(const QString &fileName, const QString &key)
{
    return QtHelperCore::getIniValue(fileName, key);
}

QString QtHelper::getIniValue(char *argv[], const QString &key, const QString &dir)
{
    return QtHelperCore::getIniValue(argv, key, dir);
}

QString QtHelper::getCompilerString()
{
    return QtHelperCore::getCompilerString();
}

QString QtHelper::getUuid()
{
    return QtHelperCore::getUuid();
}

void QtHelper::checkPath(const QString &path)
{
    QtHelperCore::checkPath(path);
}

void QtHelper::checkPaths(const QString &paths)
{
    QStringList list = paths.split("|");
    foreach (QString path, list) {
        QtHelperCore::checkPath(path);
    }
}

void QtHelper::sleep(int msec, bool exec)
{
    QtHelperCore::sleep(msec, exec);
}

void QtHelper::checkRun()
{
    QtHelperCore::checkRun();
}

void QtHelper::setStyle()
{
    QtHelperCore::setStyle();
}

void QtHelper::setCode(bool utf8)
{
    QtHelperCore::setCode(utf8);
}

QFont QtHelper::addFont(const QString &fontFile, const QString &fontName)
{
    return QtHelperCore::addFont(fontFile, fontName);
}

void QtHelper::setFont(const QString &fontFile, const QString &fontName, int fontSize)
{
    QtHelperCore::setFont(fontFile, fontName, fontSize);
}

void QtHelper::setTranslator()
{
    QtHelperCore::setTranslator();
}

void QtHelper::setTranslator(const QString &qmFile)
{
    QtHelperCore::setTranslator(qmFile);
}

bool QtHelper::checkPermission(const QString &permission)
{
    return QtHelperCore::checkPermission(permission);
}

void QtHelper::initAndroidPermission()
{
    QtHelperCore::initAndroidPermission();
}

void QtHelper::initAll(bool utf8)
{
    QtHelperCore::initAll(utf8);
}

void QtHelper::initMain(bool desktopSettingsAware, bool use96Dpi, bool logCritical)
{
    QtHelperCore::initMain(desktopSettingsAware, use96Dpi, logCritical);
}

void QtHelper::initOpenGL(quint8 type, bool checkCardEnable, bool checkVirtualSystem)
{
    QtHelperCore::initOpenGL(type, checkCardEnable, checkVirtualSystem);
}

QString QtHelper::doCmd(const QString &program, const QStringList &arguments, int timeout)
{
    return QtHelperCore::doCmd(program, arguments, timeout);
}

bool QtHelper::isVideoCardEnable()
{
    return QtHelperCore::isVideoCardEnable();
}

bool QtHelper::isVirtualSystem()
{
    return QtHelperCore::isVirtualSystem();
}

int QtHelper::strHexToDecimal(const QString &strHex)
{
    return QtHelperData::strHexToDecimal(strHex);
}

int QtHelper::strDecimalToDecimal(const QString &strDecimal)
{
    return QtHelperData::strDecimalToDecimal(strDecimal);
}

int QtHelper::strBinToDecimal(const QString &strBin)
{
    return QtHelperData::strBinToDecimal(strBin);
}

QString QtHelper::strHexToStrBin(const QString &strHex)
{
    return QtHelperData::strHexToStrBin(strHex);
}

QString QtHelper::decimalToStrBin1(int decimal)
{
    return QtHelperData::decimalToStrBin1(decimal);
}

QString QtHelper::decimalToStrBin2(int decimal)
{
    return QtHelperData::decimalToStrBin2(decimal);
}

QString QtHelper::decimalToStrHex(int decimal)
{
    return QtHelperData::decimalToStrHex(decimal);
}

QByteArray QtHelper::intToByte(int data, bool reverse)
{
    return QtHelperData::intToByte(data, reverse);
}

int QtHelper::byteToInt(const QByteArray &data, bool reverse)
{
    return QtHelperData::byteToInt(data, reverse);
}

QByteArray QtHelper::ushortToByte(int data, bool reverse)
{
    return QtHelperData::ushortToByte(data, reverse);
}

int QtHelper::byteToShort(const QByteArray &data, bool reverse)
{
    return QtHelperData::byteToShort(data, reverse);
}

QString QtHelper::getValue(quint8 value)
{
    return QtHelperData::getValue(value);
}

QString QtHelper::trimmed(const QString &text, int type)
{
    return QtHelperData::trimmed(text, type);
}

QString QtHelper::getXorEncryptDecrypt(const QString &value, char key)
{
    return QtHelperData::getXorEncryptDecrypt(value, key);
}

quint8 QtHelper::getOrCode(const QByteArray &data)
{
    return QtHelperData::getOrCode(data);
}

quint8 QtHelper::getCheckCode(const QByteArray &data)
{
    return QtHelperData::getCheckCode(data);
}

void QtHelper::getFullData(QByteArray &buffer)
{
    QtHelperData::getFullData(buffer);
}

QByteArray QtHelper::getCrcCode(const QByteArray &data)
{
    return QtHelperData::getCrcCode(data);
}

QString QtHelper::byteArrayToAsciiStr(const QByteArray &data)
{
    return QtHelperData::byteArrayToAsciiStr(data);
}

QByteArray QtHelper::asciiStrToByteArray(const QString &data)
{
    return QtHelperData::asciiStrToByteArray(data);
}

QByteArray QtHelper::hexStrToByteArray(const QString &data)
{
    return QtHelperData::hexStrToByteArray(data);
}

QString QtHelper::byteArrayToHexStr(const QByteArray &data)
{
    return QtHelperData::byteArrayToHexStr(data);
}

QString QtHelper::getOpenFileName(const QString &filter, const QString &dirName, const QString &fileName, bool native, int width, int height)
{
    return QtHelperFile::getOpenFileName(filter, dirName, fileName, native, width, height);
}

QString QtHelper::getSaveFileName(const QString &filter, const QString &dirName, const QString &fileName, bool native, int width, int height)
{
    return QtHelperFile::getSaveFileName(filter, dirName, fileName, native, width, height);
}

QString QtHelper::getExistingDirectory(const QString &dirName, bool native, int width, int height)
{
    return QtHelperFile::getExistingDirectory(dirName, native, width, height);
}

QString QtHelper::getFileNameWithExtension(const QString &fileName)
{
    return QtHelperFile::getFileNameWithExtension(fileName);
}

QStringList QtHelper::getFolderFileNames(const QStringList &filter)
{
    return QtHelperFile::getFolderFileNames(filter);
}

bool QtHelper::copyFile(const QString &sourceFile, const QString &targetFile)
{
    return QtHelperFile::copyFile(sourceFile, targetFile);
}

void QtHelper::deleteDirectory(const QString &path)
{
    QtHelperFile::deleteDirectory(path);
}

QString QtHelper::getIP(const QString &url)
{
    return QtHelperNet::getIP(url);
}

bool QtHelper::isIP(const QString &ip)
{
    return QtHelperNet::isIP(ip);
}

bool QtHelper::isMac(const QString &mac)
{
    return QtHelperNet::isMac(mac);
}

bool QtHelper::isTel(const QString &tel)
{
    return QtHelperNet::isTel(tel);
}

bool QtHelper::isEmail(const QString &email)
{
    return QtHelperNet::isEmail(email);
}

QString QtHelper::ipv4IntToString(quint32 ip)
{
    return QtHelperNet::ipv4IntToString(ip);
}

quint32 QtHelper::ipv4StringToInt(const QString &ip)
{
    return QtHelperNet::ipv4StringToInt(ip);
}

bool QtHelper::hostLive(const QString &hostName, int port, int timeout)
{
    return QtHelperNet::hostLive(hostName, port, timeout);
}

bool QtHelper::download(const QString &url, const QString &fileName, int timeout)
{
    return QtHelperNet::download(url, fileName, timeout);
}

QByteArray QtHelper::getHtml(const QString &url, int timeout)
{
    return QtHelperNet::getHtml(url, timeout);
}

QString QtHelper::getNetIP(const QString &html)
{
    return QtHelperNet::getNetIP(html);
}

QString QtHelper::getLocalIP()
{
    return QtHelperNet::getLocalIP();
}

QStringList QtHelper::getLocalIPs()
{
    return QtHelperNet::getLocalIPs();
}

QString QtHelper::urlToIP(const QString &url)
{
    return QtHelperNet::urlToIP(url);
}

void QtHelper::setFormShadow(QWidget *widget, QLayout *layout, const QString &color, int margin, int radius)
{
    QtHelperForm::setFormShadow(widget, layout, color, margin, radius);
}

void QtHelper::setFormShadow(const QString &color)
{
    QtHelperForm::setFormShadow(color);
}

void QtHelper::setFramelessForm(QWidget *widgetMain, bool tool, bool top, bool menu, bool x11)
{
    QtHelperForm::setFramelessForm(widgetMain, tool, top, menu, x11);
}

void QtHelper::setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle,
                                QLabel *labIco, QPushButton *btnClose,
                                bool tool, bool top, bool menu, bool x11)
{
    QtHelperForm::setFramelessForm(widgetMain, widgetTitle, labIco, btnClose, tool, top, menu, x11);
}

int QtHelper::showFormExec(QWidget *widget, const QString &title, bool windowModal)
{
    return QtHelperForm::showFormExec(widget, title, windowModal);
}

int QtHelper::showMessageBox(const QString &text, int type, int timeout, bool exec)
{
    return QtHelperForm::showMessageBox(text, type, timeout, exec);
}

void QtHelper::showMessageBoxInfo(const QString &text, int timeout, bool exec)
{
    QtHelperForm::showMessageBoxInfo(text, timeout, exec);
}

void QtHelper::showMessageBoxError(const QString &text, int timeout, bool exec)
{
    QtHelperForm::showMessageBoxError(text, timeout, exec);
}

int QtHelper::showMessageBoxQuestion(const QString &text)
{
    return QtHelperForm::showMessageBoxQuestion(text);
}

void QtHelper::showTipBox(const QString &title, const QString &tip, bool fullScreen, bool center, int timeout)
{
    QtHelperForm::showTipBox(title, tip, fullScreen, center, timeout);
}

void QtHelper::hideTipBox()
{
    QtHelperForm::hideTipBox();
}

QString QtHelper::showInputBox(const QString &title, int type, int timeout,
                               const QString &placeholderText, bool pwd,
                               const QString &defaultValue)
{
    return QtHelperForm::showInputBox(title, type, timeout, placeholderText, pwd, defaultValue);
}

int QtHelper::showDateSelect(QString &dateStart, QString &dateEnd, const QString &format)
{
    return QtHelperForm::showDateSelect(dateStart, dateEnd, format);
}

void QtHelper::showAboutInfo(const AboutInfo &info, int timeout, bool exec)
{
    QtHelperForm::showAboutInfo(info, timeout, exec);
}

void QtHelper::showSplashInfo(const QString &text, int fontSizeMain, int fontSizeSub, int timeout, bool exec)
{
    QtHelperForm::showSplashInfo(text, fontSizeMain, fontSizeSub, timeout, exec);
}

void QtHelper::hideSplashInfo()
{
    QtHelperForm::hideSplashInfo();
}

QPixmap QtHelper::getPixmap(QWidget *widget, const QPixmap &pixmap, bool scale)
{
    return QtHelperImage::getPixmap(widget, pixmap, scale);
}

void QtHelper::setPixmap(QLabel *label, const QString &file, bool scale)
{
    QtHelperImage::setPixmap(label, file, scale);
}

void QtHelper::setLogo(QLabel *label, const QString &file,
                       int width, int height, int offset,
                       const QString &oldColor, const QString &newColor)
{
    QtHelperImage::setLogo(label, file, width, height, offset, oldColor, newColor);
}

QRect QtHelper::getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth, int scaleMode)
{
    return QtHelperImage::getCenterRect(imageSize, widgetRect, borderWidth, scaleMode);
}

void QtHelper::getScaledImage(QImage &image, const QSize &widgetSize, int scaleMode, bool fast)
{
    QtHelperImage::getScaledImage(image, widgetSize, scaleMode, fast);
}

void QtHelper::initDb(const QString &dbName)
{
    QtHelperOther::initDb(dbName);
}

void QtHelper::initFile(const QString &sourceName, const QString &targetName)
{
    QtHelperOther::initFile(sourceName, targetName);
}

bool QtHelper::checkIniFile(const QString &iniFile)
{
    return QtHelperOther::checkIniFile(iniFile);
}

void QtHelper::setIconBtn(QAbstractButton *btn, const QString &png, int icon)
{
    QtHelperOther::setIconBtn(btn, png, icon);
}

void QtHelper::writeInfo(const QString &text, bool needWrite, const QString &filePath)
{
    QtHelperOther::writeInfo(text, needWrite, filePath);
}

void QtHelper::writeError(const QString &text, bool needWrite, const QString &filePath)
{
    QtHelperOther::writeError(text, needWrite, filePath);
}

void QtHelper::setSystemDateTime(const QString &year, const QString &month, const QString &day, const QString &hour, const QString &min, const QString &sec)
{
    QtHelperOther::setSystemDateTime(year, month, day, hour, min, sec);
}

void QtHelper::runWithSystem(bool autoRun)
{
    QtHelperOther::runWithSystem(autoRun);
}

void QtHelper::runWithSystem(const QString &fileName, const QString &filePath, bool autoRun)
{
    QtHelperOther::runWithSystem(fileName, filePath, autoRun);
}

void QtHelper::runBin(const QString &path, const QString &name)
{
    return QtHelperOther::runBin(path, name);
}

QList<QColor> QtHelper::getColorList()
{
    return QtHelperOther::getColorList();
}

QStringList QtHelper::getColorNames()
{
    return QtHelperOther::getColorNames();
}

QColor QtHelper::getRandColor()
{
    return QtHelperOther::getRandColor();
}

void QtHelper::initRand()
{
    QtHelperOther::initRand();
}

float QtHelper::getRandFloat(float min, float max)
{
    return QtHelperOther::getRandFloat(min, max);
}

double QtHelper::getRandValue(int min, int max, bool contansMin, bool contansMax)
{
    return QtHelperOther::getRandValue(min, max, contansMin, contansMax);
}

QStringList QtHelper::getRandPoint(int count, float mainLng, float mainLat, float dotLng, float dotLat)
{
    return QtHelperOther::getRandPoint(count, mainLng, mainLat, dotLng, dotLat);
}

int QtHelper::getRangeValue(int oldMin, int oldMax, int oldValue, int newMin, int newMax)
{
    return QtHelperOther::getRangeValue(oldMin, oldMax, oldValue, newMin, newMax);
}

void QtHelper::initDataTimeEdit(QDateTimeEdit *dateTimeEdit, qint64 days)
{
    QtHelperOther::initDataTimeEdit(dateTimeEdit, days);
}

void QtHelper::initTableView(QTableView *tableView, int rowHeight, bool headVisible, bool edit, bool stretchLast)
{
    QtHelperOther::initTableView(tableView, rowHeight, headVisible, edit, stretchLast);
}

void QtHelper::openFile(const QString &fileName, const QString &msg)
{
    QtHelperOther::openFile(fileName, msg);
}

bool QtHelper::checkRowCount(int rowCount, int maxCount, int warnCount)
{
    return QtHelperOther::checkRowCount(rowCount, maxCount, warnCount);
}

QString QtHelper::appendMsg(QTextEdit *textEdit, int type, const QString &data, int maxCount, int &currentCount, bool clear, bool pause)
{
    return QtHelperOther::appendMsg(textEdit, type, data, maxCount, currentCount, clear, pause);
}

QString QtHelper::cutString(const QString &text, int len, int left, int right, bool file, const QString &mid)
{
    return QtHelperOther::cutString(text, len, left, right, file, mid);
}

QString QtHelper::getTimeString(qint64 time)
{
    return QtHelperOther::getTimeString(time);
}

QString QtHelper::getTimeString(QElapsedTimer timer)
{
    return QtHelperOther::getTimeString(timer);
}

QString QtHelper::getSizeString(quint64 size)
{
    return QtHelperOther::getSizeString(size);
}
