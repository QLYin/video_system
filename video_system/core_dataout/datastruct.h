#ifndef DATASTRUCT_H
#define DATASTRUCT_H

//导入导出数据结构体
struct DataContent {
    QString html;               //可外部直接传入html
    QString fileName;           //文件名称 导出到xls和pdf需要
    QString sheetName;          //表名 导出到xls需要

    QString title;              //主标题
    QString subTitle;           //子标题

    QStringList subTitle1;      //子标题1
    QStringList subTitle2;      //子标题2

    QList<QString> columnNames; //字段名称
    QList<int> columnWidths;    //字段宽度

    int wrapText;               //自动换行 0-不换行 1-自动换行
    int autoFitHeight;          //自动高度 0-不自动 1-自动识别
    int defaultAlignment;       //默认对齐 0-居中对齐 1-左对齐 2-右对齐
    QList<int> columnAlignment; //字段对齐 0-居中对齐 1-左对齐 2-右对齐
    QList<int> contentAlignment;//内容对齐 0-居中对齐 1-左对齐 2-右对齐

    QStringList content;        //内容集合
    QString separator;          //行内容分隔符
    QString subSeparator;       //子内容分隔符

    int borderWidth;            //边框宽度
    bool autoType;              //自动填数据类型 默认开启
    bool cellStyle;             //数据单元格样式 默认不开启 不开启可以节约大概30%的文件体积
    bool randomColor;           //随机颜色
    QList<int> colorColumn;     //随机颜色列索引集合

    int checkColumn;            //校验列
    QString checkType;          //校验类型
    QString checkValue;         //校验值
    QString checkColor;         //检验颜色

    int maxCount;               //最大行数
    int warnCount;              //警告行数

    //下面的参数是打印才有
    bool stretchLast;           //最后列拉伸填充
    bool landscape;             //横向排版
    QMargins pageMargin;        //纸张边距

    //默认参数
    DataContent() {
        wrapText = 1;
        autoFitHeight = 1;
        defaultAlignment = 0;

        separator = ";";
        subSeparator = "|";

        borderWidth = 1;
        autoType = true;
        cellStyle = false;
        randomColor = false;

        checkColumn = -1;
        checkType = "==";
        checkValue = "0";
        checkColor = "#FF0000";

        maxCount = 100000;
        warnCount = 10000;

        stretchLast = true;
        landscape = false;
        pageMargin = QMargins(10, 12, 10, 12);
    }
};

//无人机报告内容结构体
struct UavsReportData {
    QString title;          //大标题
    QString subTitle;       //子标题

    QString name;           //设备名称
    QString lng;            //坐标经度值
    QString lat;            //坐标纬度值
    QString timeAlarm;      //报警触发时间
    QString timeReport;     //报告打印时间

    QString type;           //警情类型
    QString textCount;      //报警设备数量
    QString textLevel;      //报警级别
    QStringList images;     //报警图片集合

    UavsReportData() {
        title = QString::fromUtf8("无人机监控系统告警报告");
        subTitle = QString::fromUtf8("告警结果详情");

        name = QString::fromUtf8("无人机-001");
        lng = "121.414005";
        lat = "31.182805";
        timeAlarm = DATETIME;
        timeReport = DATETIME;

        type = QString::fromUtf8("可疑人员、可疑车辆、可疑物品");
        textCount = QString::fromUtf8("10个");
        textLevel = QString::fromUtf8("一级");
    }
};

//个人信息报告内容结构体
struct PersonReportData {
    QString title;          //标题
    QString time;           //时间

    QString columnWidth1;   //字段宽度1
    QString columnWidth2;   //字段宽度2
    QString columnWidth3;   //字段宽度3    
    QStringList columnName; //字段名称

    PersonReportData() {
        title = "个人信息表";
        time = DATETIME;

        columnWidth1 = "15%";
        columnWidth2 = "65%";
        columnWidth3 = "20%";

        columnName << "工号: " << "姓名: " << "性别: " << "政治面貌: " << "文化程度: ";
        columnName << "生日: " << "婚否: " << "健康状况: " << "部门: " << "职务: ";
        columnName << "工段: " << "工种: " << "工龄: " << "职工类型: " << "合同开始: ";
        columnName << "合同终止: " << "身份证号: " << "联系电话: " << "家庭住址: ";
        columnName << "亲属姓名: " << "亲属关系: " << "亲属电话: " << "备注: " << "头像";
    }
};

#endif // DATASTRUCT_H
