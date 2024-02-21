#include "dbpage.h"

DbPage::DbPage(QObject *parent) : QObject(parent)
{
    startIndex = 0;
    queryModel = new DbPageModel;

    pageCurrent = 1;
    pageTotal = 0;
    recordsTotal = 0;
    recordsPerpage = 0;

    labPageTotal = 0;
    labPageCurrent = 0;
    labRecordsTotal = 0;
    labRecordsPerpage = 0;
    labSelectTime = 0;
    labSelectInfo = 0;

    tableView = 0;
    btnFirst = 0;
    btnPrevious = 0;
    btnNext = 0;
    btnLast = 0;

    connName = "qt_sql_default_connection";
    dbType = DbType_Sqlite;

    pageCurrent = 0;
    pageTotal = 0;
    recordsTotal = 0;
    recordsPerpage = 30;

    tableName = "LogInfo";
    selectColumn = "*";
    orderSql = "rowid asc";
    whereSql = "1=1";

    columnNames.clear();
    columnWidths.clear();

    insertColumnIndex = -1;
    insertColumnName = "";
    insertColumnWidth = 50;
}

quint32 DbPage::getPageCurrent() const
{
    return this->pageCurrent;
}

quint32 DbPage::getPageTotal() const
{
    return this->pageTotal;
}

quint32 DbPage::getRecordsTotal() const
{
    return this->recordsTotal;
}

quint32 DbPage::getRecordsPerpage() const
{
    return this->recordsPerpage;
}

void DbPage::bindData(const QString &sql)
{
    //计时
    QElapsedTimer timer;
    timer.start();

    queryModel->setQuery(sql, QSqlDatabase::database(connName));
    tableView->setModel(queryModel);

    //依次设置列标题列宽
    int columnCount = tableView->model()->columnCount();
    int nameCount = columnNames.count();
    columnCount = columnCount > nameCount ? nameCount : columnCount;

    QList<QString> columnNames = this->columnNames;
    QList<int> columnWidths = this->columnWidths;

    //根据设置添加新列,将对应新列的标题名称和宽度按照索引位置插
    if (insertColumnIndex >= 0) {
        columnCount++;
        columnNames.insert(insertColumnIndex, insertColumnName);
        columnWidths.insert(insertColumnIndex, insertColumnWidth);
        queryModel->insertColumn(insertColumnIndex);
    }

    //设置列标题和列宽度
    for (int i = 0; i < columnCount; ++i) {
        queryModel->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        tableView->setColumnWidth(i, columnWidths.at(i));
    }

    if (labPageCurrent) {
        labPageCurrent->setText(QString("第 %1 页").arg(pageCurrent));
    }

    if (labPageTotal) {
        labPageTotal->setText(QString("共 %1 页").arg(pageTotal));
    }

    if (labRecordsTotal) {
        labRecordsTotal->setText(QString("共 %1 条").arg(recordsTotal));
    }

    if (labRecordsPerpage) {
        labRecordsPerpage->setText(QString("每页 %1 条").arg(recordsPerpage));
    }

    if (labSelectTime) {
        labSelectTime->setText(QString("查询用时 %1 秒").arg(QString::number((double)timer.elapsed() / 1000, 'f', 3)));
    }

    if (labSelectInfo) {
        //labSelectInfo->setText(QString("共 %1 条  每页 %2 条  共 %3 页  第 %4 页").arg(recordsTotal).arg(recordsPerpage).arg(pageTotal).arg(pageCurrent));
        labSelectInfo->setText(QString("第 %1 页  每页 %2 条  共 %3 页  共 %4 条").arg(pageCurrent).arg(recordsPerpage).arg(pageTotal).arg(recordsTotal));
    }

    //发送结果信号
    if (recordsTotal != recordsPerpage) {
        emit receivePage(pageCurrent, pageTotal, recordsTotal, recordsPerpage);
        //qDebug() << TIMEMS << startIndex << pageCurrent << pageTotal << recordsTotal << recordsPerpage;
    }

    changeBtnEnable();
}

QString DbPage::getPageSql()
{
    //1. 条件字段+排序字段强烈建议新建索引,并尽量用整数类型
    //2. 在已有索引的基础上,条件语句推荐对索引字段进行判断比如 where id >= 100

    //selectColumn = "LogID,TriggerTime,UserName,LogCh,LogType,LogContent,LogMark";
    QString sqlFrom = QString("from %1").arg(tableName);
    QString sqlOrder = orderSql.isEmpty() ? "" : QString(" order by %1").arg(orderSql);
    QString sqlSelect = QString("select %1 %2 %3%4").arg(selectColumn).arg(sqlFrom).arg(whereSql).arg(sqlOrder);

    //组织分页SQL语句,不同的数据库分页语句不一样
    QString sql;
    if (dbType == DbType_Sqlite || dbType == DbType_MySql) {
        //https://blog.csdn.net/jerry11112/article/details/123901346
        //索引字段如果没有指定排序字段则取默认隐藏列
        QString id = (dbType == DbType_Sqlite ? "rowid" : "_rowid");
        if (!orderSql.isEmpty()) {
            //如果排序列字段是主键则可以用下面这行
            //id = orderSql.split(" ").first();
        }

        //下面这个方法性能最差,尤其是页码很多的时候翻到后面极其糟糕
        //select * from loginfo limit 10000,100
        //sql = QString("%1 limit %2,%3").arg(sqlSelect).arg(startIndex).arg(recordsPerpage);

        //下面这个方法在页码很小的时候性能差,页码很大的时候性能高
        //select * from loginfo where logid >= (select logid from loginfo limit 10000,1) limit 100
        //sql = QString("select %1 %2 where %0 >= (select %0 %2 %3%4 limit %5,1) limit %6").arg(id).arg(selectColumn).arg(sqlFrom).arg(whereSql).arg(sqlOrder).arg(startIndex).arg(recordsPerpage);

        //下面这个方法在页码小和大的时候性能都是爆表,几千万的数据量杠杠的
        //下面的 a.*表示该表所有字段 a.c1,c2 表示指定字段
        //select a.* from loginfo a join (select logid from loginfo limit 10000,100) b on a.logid = b.logid
        sql = QString("select a.%1 %2 a join (select %0 %2 %3%4 limit %5,%6) b on a.%0 = b.%0").arg(id).arg(selectColumn).arg(sqlFrom).arg(whereSql).arg(sqlOrder).arg(startIndex).arg(recordsPerpage);
    } else if (dbType == DbType_PostgreSQL || dbType == DbType_KingBase) {
        //select * from loginfo limit 100 offset 10000
        sql = QString("%1 limit %3 offset %2").arg(sqlSelect).arg(startIndex).arg(recordsPerpage);
    } else if (dbType == DbType_SqlServer) {
        //https://www.cnblogs.com/fengxiaojiu/p/7994124.html
        //select * from loginfo order by logid offset 10000 rows fetch next 100 rows only
        sql = QString("%1 offset %2 rows fetch next %3 rows only").arg(sqlSelect).arg(startIndex).arg(recordsPerpage);
    } else if (dbType == DbType_Oracle) {
        //https://blog.csdn.net/lchmyhua88/article/details/121076227
        //select * from (select rownum r, t.* from loginfo t) where r between 10000 AND (100+10000)
        //select * from (select rownum r, t.* from loginfo t where rownum <= (100+10000)) where r >= 10000

        //如果查询所有列 * 则下面这种方式会出现rownum列
        //sql = QString("select %3 from (select rownum r, t.* %4 t where rownum <= %2%5) where r > %1").arg(startIndex).arg(startIndex + recordsPerpage).arg(selectColumn).arg(sqlFrom).arg(sqlOrder);
        sql = QString("select b.%0 from (select rowid rn, rownum r %3 where rownum <= %2%4) a, %5 b where a.rn = b.rowid and r > %1%4").arg(selectColumn).arg(startIndex).arg(startIndex + recordsPerpage).arg(sqlFrom).arg(sqlOrder).arg(tableName);

        //oracle12 开始可以用下面的方法,和sqlserver数据库一样
        //sql = QString("%1 offset %2 rows fetch next %3 rows only").arg(sqlSelect).arg(startIndex).arg(recordsPerpage);
    } else if (dbType == DbType_Access) {
        //https://blog.csdn.net/hangl_ciom/article/details/98473409
        //矫正对应的数量
        quint32 count = recordsPerpage;
        quint32 total = (startIndex + recordsPerpage);
        if (total > recordsTotal) {
            count = count - (total - recordsTotal);
            total = recordsTotal;
        }

        //排序按照原有的倒过来
        QString sqlOrder2 = sqlOrder;
        if (sqlOrder2.contains("desc")) {
            sqlOrder2.replace("desc", "asc");
        } else if (sqlOrder2.contains("asc")) {
            sqlOrder2.replace("asc", "desc");
        }

        sql = QString("select %1 from (select top %5 %1 from (select top %6 %1 %2 %3%4)%7)%4").arg(selectColumn).arg(sqlFrom).arg(whereSql).arg(sqlOrder).arg(count).arg(total).arg(sqlOrder2);
    }

    //如果为空说明还没有设定好采用何种分页算法,索性直接就按照不分页的情况
    if (sql.isEmpty()) {
        sql = sqlSelect;
    }

    return sql;
}

void DbPage::receiveCount(const QString &tag, int count, int msec)
{
    //过滤不是当前表返回的记录数
    if (tag != tableName) {
        return;
    }

    if (labSelectTime) {
        labSelectTime->setText(QString("查询用时 %1 秒").arg(QString::number((double)msec / 1000, 'f', 3)));
    }

    recordsTotal = count;
    int yushu = recordsTotal % recordsPerpage;

    //不存在余数,说明是整行,例如300%5==0
    if (yushu == 0) {
        if (recordsTotal > 0 && recordsTotal < recordsPerpage) {
            pageTotal = 1;
        } else {
            pageTotal = recordsTotal / recordsPerpage;
        }
    } else {
        pageTotal = (recordsTotal / recordsPerpage) + 1;
    }

    bindData(getPageSql());
}

//设置显示数据的表格控件,当前翻页信息的标签控件等
void DbPage::setControl(QTableView *tableView,
                        QLabel *labPageTotal, QLabel *labPageCurrent,
                        QLabel *labRecordsTotal, QLabel *labRecordsPerpage,
                        QLabel *labSelectTime, QLabel *labSelectInfo,
                        QAbstractButton *btnFirst, QAbstractButton *btnPrevious,
                        QAbstractButton *btnNext, QAbstractButton *btnLast)
{
    this->tableView = tableView;
    this->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    this->labPageTotal = labPageTotal;
    this->labPageCurrent = labPageCurrent;
    this->labRecordsTotal = labRecordsTotal;
    this->labRecordsPerpage = labRecordsPerpage;
    this->labSelectTime = labSelectTime;
    this->labSelectInfo = labSelectInfo;

    this->btnFirst = btnFirst;
    this->btnPrevious = btnPrevious;
    this->btnNext = btnNext;
    this->btnLast = btnLast;
    if (btnFirst == 0 || btnPrevious == 0 || btnNext == 0 || btnLast == 0) {
        return;
    }

    //挂载翻页按钮事件
    connect(btnFirst, SIGNAL(clicked()), this, SLOT(first()));
    connect(btnPrevious, SIGNAL(clicked()), this, SLOT(previous()));
    connect(btnNext, SIGNAL(clicked()), this, SLOT(next()));
    connect(btnLast, SIGNAL(clicked()), this, SLOT(last()));
}

void DbPage::setControl(QTableView *tableView,
                        QLabel *labPageTotal, QLabel *labPageCurrent,
                        QLabel *labRecordsTotal, QLabel *labRecordsPerpage,
                        QLabel *labSelectTime, QLabel *labSelectInfo)
{
    setControl(tableView, labPageTotal, labPageCurrent, labRecordsTotal, labRecordsPerpage, labSelectTime, labSelectInfo, 0, 0, 0, 0);
}

void DbPage::setControl(QTableView *tableView,
                        QAbstractButton *btnFirst, QAbstractButton *btnPrevious,
                        QAbstractButton *btnNext, QAbstractButton *btnLast)
{
    setControl(tableView, 0, 0, 0, 0, 0, 0, btnFirst, btnPrevious, btnNext, btnLast);
}

void DbPage::setControl(QTableView *tableView)
{
    setControl(tableView, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void DbPage::setConnName(const QString &connName)
{
    this->connName = connName;
}

void DbPage::setDbType(const QString &dbType)
{
    this->dbType = DbHelper::getDbType(dbType.toUpper());
}

void DbPage::setDbType(const DbType &dbType)
{
    this->dbType = dbType;
}

void DbPage::setTableName(const QString &tableName)
{
    this->tableName = tableName;
}

void DbPage::setSelectColumn(const QString &selectColumn)
{
    this->selectColumn = selectColumn;
}

void DbPage::setOrderSql(const QString &orderSql)
{
    this->orderSql = orderSql;
}

void DbPage::setWhereSql(const QString &whereSql)
{
    this->whereSql = whereSql;
}

void DbPage::setRecordsPerpage(int recordsPerpage)
{
    this->recordsPerpage = recordsPerpage;
}

void DbPage::setColumnNames(const QList<QString> &columnNames)
{
    this->columnNames = columnNames;
}

void DbPage::setColumnWidths(const QList<int> &columnWidths)
{
    this->columnWidths = columnWidths;
}

void DbPage::setAllCenter(bool allCenter)
{
    queryModel->setAllCenter(allCenter);
}

void DbPage::setAlignCenterColumn(const QList<int> &alignCenterColumn)
{
    queryModel->setAlignCenterColumn(alignCenterColumn);
}

void DbPage::setAlignRightColumn(const QList<int> &alignRightColumn)
{
    queryModel->setAlignRightColumn(alignRightColumn);
}

void DbPage::setInsertColumnIndex(int insertColumnIndex)
{
    this->insertColumnIndex = insertColumnIndex;
}

void DbPage::setInsertColumnName(const QString &insertColumnName)
{
    this->insertColumnName = insertColumnName;
}

void DbPage::setInsertColumnWidth(int insertColumnWidth)
{
    this->insertColumnWidth = insertColumnWidth;
}

void DbPage::changeBtnEnable()
{
    if (btnFirst == 0 || btnPrevious == 0 || btnNext == 0 || btnLast == 0) {
        return;
    }

    //下面默认对上一页下一页按钮禁用
    //也可以取消注释对第一页末一页同样处理
    //因为到了第一页就可以不必再单击第一页和上一页
    if (pageTotal <= 1) {
        //如果只有一页数据则翻页按钮不可用
        btnFirst->setEnabled(false);
        btnLast->setEnabled(false);
        btnPrevious->setEnabled(false);
        btnNext->setEnabled(false);
    } else {
        //判断是否在首页末页禁用按钮
        bool first = (pageCurrent == 1);
        bool last = (pageCurrent == pageTotal);
        btnFirst->setEnabled(!first);
        btnLast->setEnabled(!last);
        btnPrevious->setEnabled(!first);
        btnNext->setEnabled(!last);
    }
}

void DbPage::clear()
{
    tableView->setModel(NULL);
}

void DbPage::select()
{
    //重置开始索引
    startIndex = 0;
    pageCurrent = 1;
    pageTotal = 1;
    changeBtnEnable();

    //假设只有一页
    receiveCount(tableName, recordsPerpage, 0);

    //文本显示正在查询...
    QString info = "正在查询...";
    if (labSelectInfo) {
        labSelectInfo->setText(info);
    }
    if (labPageCurrent) {
        labPageCurrent->setText(info);
    }
    if (labPageTotal) {
        labPageTotal->setText(info);
    }
    if (labRecordsTotal) {
        labRecordsTotal->setText(info);
    }
    if (labRecordsPerpage) {
        labRecordsPerpage->setText(info);
    }
    if (labSelectTime) {
        labSelectTime->setText(info);
    }
}

void DbPage::selectPage(int page)
{
    //必须小于总页数+不是当前页
    if (page >= 1 && page <= pageTotal && page != pageCurrent) {
        //计算指定页对应开始的索引
        startIndex = (page - 1) * recordsPerpage;
        pageCurrent = page;
        bindData(getPageSql());
    }
}

void DbPage::first()
{
    //当前页不是第一页才能切换到第一页
    if (pageTotal > 1 && pageCurrent != 1) {
        startIndex = 0;
        pageCurrent = 1;
        bindData(getPageSql());
    }
}

void DbPage::previous()
{
    //当前页不是第一页才能上一页
    if (pageCurrent > 1) {
        pageCurrent--;
        startIndex -= recordsPerpage;
        bindData(getPageSql());
    }
}

void DbPage::next()
{
    //当前页小于总页数才能下一页
    if (pageCurrent < pageTotal) {
        pageCurrent++;
        startIndex += recordsPerpage;
        bindData(getPageSql());
    }
}

void DbPage::last()
{
    //当前页不是末尾页才能切换到末尾页
    if (pageTotal > 1 && pageCurrent != pageTotal) {
        startIndex = (pageTotal - 1) * recordsPerpage;
        pageCurrent = pageTotal;
        bindData(getPageSql());
    }
}
