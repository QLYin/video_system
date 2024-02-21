#include "webjsdata.h"
#include "qstringlist.h"
#include "qvariant.h"
#include "qdebug.h"

WebJsData *WebJsData::Instance()
{
    static WebJsData self;
    return &self;
}

WebJsData::WebJsData(QObject *parent)
{
}

void WebJsData::receiveData(const QString &type, const QVariant &data)
{
    //可以在这里重新梳理好再发出去信号
    emit receiveDataFromJs(type, data);
}
