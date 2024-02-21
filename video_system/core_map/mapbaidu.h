#ifndef MAPBAIDU_H
#define MAPBAIDU_H

#include <QObject>
#include <QStringList>

class MapBaiDu : public QObject
{
    Q_OBJECT
public:
    enum OverlayType {
        OverlayType_Marker = 0,         //点
        OverlayType_Polyline = 1,       //折线
        OverlayType_Polygon = 2,        //多边形
        OverlayType_Rectangle = 3,      //矩形
        OverlayType_Circle = 4,         //圆形
        OverlayType_CurveLine = 5,      //弧线
        OverlayType_Boundary = 6,       //行政区划
        OverlayType_MarkerClusterer = 7 //点聚合
    };

    static MapBaiDu *Instance();
    explicit MapBaiDu(QObject *parent = 0);

private:
    static QScopedPointer<MapBaiDu> self;

    int height;                     //高度
    bool mapLocal;                  //离线地图
    bool saveFile;                  //保存到文件
    bool addFun;                    //添加动态函数

    QString title;                  //网页标题
    QString fileName;               //文件名称
    QString cityJsName;             //行政区划边界点集合城市js文件名称

    QString mapFlag;                //地图标识 BMap BMapGL
    QString mapVersionKey;          //API协议版本及密钥
    QString mapStyleName;           //地图主题样式
    QString mapCenterPoint;         //地图中心坐标
    QString mapCenterCity;          //地图中心城市

    quint8 mapZoom;                 //地图缩放比例
    quint8 mapMinZoom;              //地图缩放最小级别
    quint8 mapMaxZoom;              //地图缩放最大级别

    bool enableMapClick;            //地图可单击
    bool enableDragging;            //启用地图拖拽
    bool enableScrollWheelZoom;     //启用滚轮放大缩小
    bool enableDoubleClickZoom;     //启用鼠标双击放大
    bool enableKeyboard;            //启用键盘移动
    bool enableAnimation;           //标识跳动
    bool enableClickPoint;          //单击获取鼠标处经度纬度
    bool enableDistance;            //进入测距状态

    bool showNavigationControl;     //显示缩放控件
    bool showOverviewMapControl;    //显示缩略图控件
    bool showScaleControl;          //显示比例尺控件
    bool showMapTypeControl;        //显示地图类型控件
    bool showPanoramaCoverageLayer; //显示全景控件
    bool showTrafficControl;        //显示路况控件
    bool showOverlayTool;           //显示点线面工具

    quint8 routeType;               //查询路线方式
    quint8 policyType;              //路线方案
    QString startAddr;              //起点地址
    QString endAddr;                //终点地址

    QStringList markerNames;        //标注名称集合
    QStringList markerAddrs;        //标注地址集合
    QStringList markerPoints;       //标注坐标集合

    QString strokeColor;            //覆盖物颜色
    int strokeWeight;               //覆盖物边框
    float strokeOpacity;            //透明度

    //覆盖物类型集合 0-点 1-折线 2-多边形 3-矩形 4-圆形
    QList<OverlayType> overlayTypes;
    //覆盖物坐标集合
    QList<QStringList> overlayPoints;

private:
    //添加头部数据
    void addHead(QStringList &list);
    //添加网页body
    void addBody(QStringList &list);
    //添加地图相关属性
    void addProperty(QStringList &list);
    //添加js函数,可以Qt程序动态交互而不需要刷新
    void addFunction(QStringList &list);

    //通用多个点坐标转换成 BMap.Point 对象
    void getPoints(QStringList &list);
    //通用多个点坐标转换成 BMap.Marker 对象
    void getMarkers(QStringList &list);
    //将路径转换成坐标点集合
    void getPathPoints(QStringList &list);
    //获取可视区域经纬度
    void getBounds(QStringList &list);
    //获取边界点集合
    void getBoundary(QStringList &list);
    //获取当前地图缩放级别
    void getZoom(QStringList &list);

    //其他方法
    void addOther(QStringList &list);
    //地址解析成坐标+坐标解析成地址
    void addGeocoder(QStringList &list);
    //本地搜索
    void addLocalSearch(QStringList &list);
    //自定义方法显示标注详细信息,可以改成自己的内容,标准html格式
    void addClick(QStringList &list);

    //动态添加点
    void addMarker(QStringList &list);
    //动态移动点
    void moveMarker(QStringList &list);
    //动态删除点
    void deleteMarker(QStringList &list);
    //动态删除覆盖物
    void deleteOverlay(QStringList &list);
    //获取覆盖物信息
    void getOverlayInfo(QStringList &list);

    //覆盖物动态属性
    void getProperty(QStringList &list);
    //动态添加折线
    void addPolyline(QStringList &list);
    //动态添加多边形
    void addPolygon(QStringList &list);
    //动态添加矩形
    void addRectangle(QStringList &list);
    //动态添加圆形
    void addCircle(QStringList &list);
    //动态添加弧线
    void addCurveLine(QStringList &list);

    //动态添加行政区划
    void addBoundary(QStringList &list);
    //动态添加点聚合
    void addMarkerClusterer(QStringList &list);
    //动态添加海量点
    void addPointCollection(QStringList &list);

    //添加设备点集合
    void addDevice(QStringList &list);
    //添加浮动工具栏
    void addOverlayTool(QStringList &list);
    //覆盖物的属性
    QString getOverlayProperty();
    //添加覆盖层
    void addOverlay(QStringList &list);

    //添加公交路线+驾车路线+路线方案
    void addRoute(QStringList &list);
    //添加尾部数据
    void addEnd(QStringList &list);

public slots:
    //重置初始值
    void reset();
    //生成地图文件
    QString newMap();

    //设置画布高度+离线地图模式+保存到文件+标题+文件名
    void setHeight(int height);
    void setMapLocal(bool mapLocal);
    void setSaveFile(bool saveFile);
    bool getSaveFile();
    void setTitle(const QString &title);
    void setFileName(const QString &fileName);

    //设置城市边界点js文件名称
    void setCityJsName(const QString &cityJsName);

    //设置地图类型标识
    void setMapFlag(const QString &mapFlag);
    //设置协议版本和秘钥
    void setMapVersionKey(const QString &mapVersionKey);
    //设置主题样式-2019年后该模块官方要收费
    void setMapStyleName(const QString &mapStyleName);
    //设置地图中心坐标
    void setMapCenterPoint(const QString &mapCenterPoint);
    //设置地图中心城市
    void setMapCenterCity(const QString &mapCenterCity);

    //设置缩放级别+级别范围
    void setMapZoom(quint8 mapZoom);
    quint8 getMapZoom();
    void setMapMinZoom(quint8 mapMinZoom);
    void setMapMaxZoom(quint8 mapMaxZoom);

    //设置地图可单击+拖动+滚轮缩放等
    void setEnableMapClick(bool enableMapClick);
    void setEnableDragging(bool enableDragging);
    void setEnableScrollWheelZoom(bool enableScrollWheelZoom);
    void setEnableDoubleClickZoom(bool enableDoubleClickZoom);
    void setEnableKeyboard(bool enableKeyboard);
    void setEnableAnimation(bool enableAnimation);
    void setEnableClickPoint(bool enableClickPoint);

    //设置缩放+缩略图+比例尺+路况+点线面工具面板等控件是否启用或者可见
    void setShowNavigationControl(bool showNavigationControl);
    void setShowOverviewMapControl(bool showOverviewMapControl);
    void setShowScaleControl(bool showScaleControl);
    void setShowMapTypeControl(bool showMapTypeControl);
    void setShowPanoramaCoverageLayer(bool showPanoramaCoverageLayer);
    void setShowTrafficControl(bool showTrafficControl);
    void setShowOverlayTool(bool showOverlayTool);

    //设置覆盖物的颜色+边框+透明度
    void setStrokeColor(const QString &strokeColor);
    void setStrokeWeight(int strokeWeight);
    void setStrokeOpacity(float strokeOpacity);

    //设置标注信息集合,包括名称+地址+经纬度
    void setMarkerInfo(const QStringList &markerNames, const QStringList &markerAddrs, const QStringList &markerPoints);
    //设置查询路线信息
    void setRotueInfo(quint8 routeType, quint8 policyType, const QString &startAddr, const QString &endAddr);
    //设置覆盖物信息
    void setOverlayInfo(QList<OverlayType> &overlayTypes, const QList<QStringList> &overlayPoints);
};

#endif // MAPBAIDU_H
