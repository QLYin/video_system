#pragma execution_character_set("utf-8")
#include "mapbaidu.h"
#include "qmutex.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qdebug.h"

QScopedPointer<MapBaiDu> MapBaiDu::self;
MapBaiDu *MapBaiDu::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new MapBaiDu);
        }
    }

    return self.data();
}

MapBaiDu::MapBaiDu(QObject *parent) : QObject(parent)
{
    this->reset();
}

void MapBaiDu::addHead(QStringList &list)
{
    //构建网页头部
    list << QString("<html>");
    list << QString("<head>");
    list << QString("<title>%1</title>").arg(title);
    list << QString("<meta charset=\"utf-8\">");
    list << QString("<meta name=\"viewport\" content=\"initial-scale=1, maximum-scale=1, user-scalable=no, width=device-width\">");

    //地图页面样式
    list << QString("<style type=\"text/css\">");
    //全局字体+页面宽度高度占比+边距等
    list << QString("  html,body{font-family:微软雅黑;height:100%;width:100%;margin:0px;padding:0px;}");
    //隐藏左下角的logo
    list << QString("  .anchorBL{opacity:0;}");

    if (startAddr.isEmpty()) {
        list << QString("  #map{height:100%;width:100%;}");
    } else {
        list << QString("  #map{height:%1px;width:100%;}").arg(height);
        list << QString("  #result,#result table{width:100%;font-size:12px;}");
    }

    //webkit浏览器滚动条样式
    list << QString("  ::-webkit-scrollbar{width:0.8em;}");
    list << QString("  ::-webkit-scrollbar-track{background:rgb(241,241,241);}");
    list << QString("  ::-webkit-scrollbar-thumb{background:rgb(188,188,188);}");
    list << QString("</style>");

    //引入webchannel.js
#ifdef webengine
    list << QString("<script type=\"text/javascript\" src=\"qwebchannel.js\"></script>");
#endif

    //在线和离线地图加载的文件路径不一样
    if (mapLocal) {
        //引入地图JS文件
        list << QString("<script type=\"text/javascript\" src=\"map_load.js\"></script>");

        //引入弧线JS文件,只有当绘制弧线的时候才需要,如果不需要绘制弧线可以注释
        list << QString("<script type=\"text/javascript\" src=\"tools/CurveLine.min.js\"></script>");

        //加载点聚合需要下面两个JS文件
        list << QString("<script type=\"text/javascript\" src=\"tools/TextIconOverlay_min.js\"></script>");
        list << QString("<script type=\"text/javascript\" src=\"tools/MarkerClusterer_min.js\"></script>");

        //引入行政区划轮廓图JS文件
        list << QString("<script type=\"text/javascript\" src=\"citypointjs/%1.js\"></script>").arg(cityJsName);

        //引入鼠标绘制工具JS文件
        if (showOverlayTool) {
            list << QString("<script type=\"text/javascript\" src=\"tools/DrawingManager_min.js\"></script>");
            list << QString("<link rel=\"stylesheet\" type=\"text/css\" href=\"tools/DrawingManager_min.css\"/>");
        }

        //引入测距JS文件
        list << QString("<script type=\"text/javascript\" src=\"tools/DistanceTool_min.js\"></script>");
    } else {
        //引入地图JS文件
        if (mapFlag == "BMapGL") {
            list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/api?type=webgl&v=%1\"></script>").arg(mapVersionKey);
        } else {
            list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/api?v=%1\"></script>").arg(mapVersionKey);
        }

        //引入弧线JS文件,只有当绘制弧线的时候才需要,如果不需要绘制弧线可以注释
        list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/library/CurveLine/1.5/src/CurveLine.min.js\"></script>");

        //加载点聚合需要下面两个JS文件
        list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/library/TextIconOverlay/1.2/src/TextIconOverlay_min.js\"></script>");
        list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/library/MarkerClusterer/1.2/src/MarkerClusterer_min.js\"></script>");

        //引入轨迹图动画JS文件
        list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/library/TrackAnimation/src/TrackAnimation_min.js\"></script>");

        //引入实时路况JS文件
        if (showTrafficControl) {
            list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/library/TrafficControl/1.4/src/TrafficControl_min.js\"></script>");
            list << QString("<link rel=\"stylesheet\" type=\"text/css\" href=\"http://api.map.baidu.com/library/TrafficControl/1.4/src/TrafficControl_min.css\"/>");
        }

        //引入鼠标绘制工具JS文件
        if (showOverlayTool) {
            list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/library/DrawingManager/1.4/src/DrawingManager_min.js\"></script>");
            list << QString("<link rel=\"stylesheet\" type=\"text/css\" href=\"http://api.map.baidu.com/library/DrawingManager/1.4/src/DrawingManager_min.css\"/>");
        }

        //引入测距JS文件
        list << QString("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/library/DistanceTool/1.2/src/DistanceTool_min.js\"></script>");
    }

    list << QString("</head>");
}

void MapBaiDu::addBody(QStringList &list)
{
    //构建网页主体部分内容
    list << QString("<body>");
    list << QString("  <div id='map'></div>");

    if (!startAddr.isEmpty()) {
        list << QString("  <div id='result'></div>");
    }

    list << QString("</body>");

    //引入js文件
    list << QString("<script type=\"text/javascript\">");
    //通用返回数据JS函数
    list << QString("  function receiveData(type, data) {");
#ifdef webminiblink
    list << QString("    objName_receiveData(type, data);");
#else
    list << QString("    objName.receiveData(type, data);");
#endif
    list << QString("  }");

    //生成QWebChannel通信对象
    //重复传入对象可能会提示 js: Uncaught TypeError: Cannot read property 'receiveDataFromJs' of undefined
#ifndef webkit
#ifdef webengine
    list << QString("  new QWebChannel(qt.webChannelTransport, function(channel){window.objName = channel.objects.objName;})");
#endif
#endif
}

void MapBaiDu::addProperty(QStringList &list)
{
    //定义地图对象
    //可设置默认街道图还是卫星图等 街道图: BMAP_NORMAL_MAP 卫星图: BMAP_SATELLITE_MAP 混合图: BMAP_HYBRID_MAP
    list << QString("  var map = new %1.Map('map', {minZoom:%2, maxZoom:%3, enableMapClick:%4, mapType:%5});")
         .arg(mapFlag).arg(mapMinZoom).arg(mapMaxZoom).arg(enableMapClick ? "true" : "false").arg("BMAP_NORMAL_MAP");
    list << QString("  var point = new %1.Point(%2);").arg(mapFlag).arg(mapCenterPoint);

    //初始化地图,设置中心点坐标或者中心城市和地图级别.优先按照中心坐标设置
    //离线地图需要采用中心点坐标的形式
    if (!mapCenterPoint.isEmpty()) {
        list << QString("  map.centerAndZoom(point, %1);").arg(mapZoom);
    } else {
        list << QString("  map.centerAndZoom(\"%1\", %2);").arg(mapCenterCity).arg(mapZoom);
    }

    //启用地图拖拽事件,默认启用(可不写)
    if (enableDragging) {
        list << QString("  map.enableDragging();");
    }
    //启用滚轮放大缩小,默认禁用
    if (enableScrollWheelZoom) {
        list << QString("  map.enableScrollWheelZoom();");
    }
    //启用鼠标双击放大,默认启用(可不写)
    if (enableDoubleClickZoom) {
        list << QString("  map.enableDoubleClickZoom();");
    }
    //启用键盘移动,默认禁用
    if (enableKeyboard) {
        list << QString("  map.enableKeyboard();");
    }

    //启用地图惯性拖拽,默认禁用
    list << QString("  map.enableInertialDragging();");
    //启用连续缩放效果,默认禁用
    list << QString("  map.enableContinuousZoom();");
    //启用双指操作缩放,默认启用
    list << QString("  map.enablePinchToZoom();");
    //启用自动适应容器尺寸变化,默认启用
    list << QString("  map.enableAutoResize();");

    //向地图中添加缩放控件
    if (showNavigationControl) {
        list << QString("  map.addControl(new %1.NavigationControl({anchor:BMAP_ANCHOR_TOP_LEFT, type:BMAP_NAVIGATION_CONTROL_LARGE}));").arg(mapFlag);
    }

    //向地图中添加缩略图控件
    if (showOverviewMapControl) {
        list << QString("  map.addControl(new %1.OverviewMapControl({anchor:BMAP_ANCHOR_BOTTOM_RIGHT, isOpen:1}));").arg(mapFlag);
    }

    //向地图中添加比例尺控件
    if (showScaleControl) {
        list << QString("  map.addControl(new %1.ScaleControl({anchor:BMAP_ANCHOR_BOTTOM_LEFT}));").arg(mapFlag);
    }

    //添加地图类型控件(type: 按钮-BMAP_MAPTYPE_CONTROL_HORIZONTAL/下拉-BMAP_MAPTYPE_CONTROL_DROPDOWN/图片-BMAP_MAPTYPE_CONTROL_MAP)
    if (showMapTypeControl) {
        list << QString("  map.addControl(new %1.MapTypeControl({type:BMAP_MAPTYPE_CONTROL_HORIZONTAL, mapTypes:[BMAP_NORMAL_MAP,BMAP_SATELLITE_MAP,BMAP_HYBRID_MAP]}));").arg(mapFlag);
    }

    //添加全景控件
    if (showPanoramaCoverageLayer) {
        list << QString("  map.addTileLayer(new %1.PanoramaCoverageLayer());").arg(mapFlag);
        list << QString("  var ctrlPan = new %1.PanoramaControl();").arg(mapFlag);
        list << QString("  ctrlPan.setOffset(new %1.Size(20, 50));").arg(mapFlag);
        list << QString("  map.addControl(ctrlPan);");
    }

    //添加路况控件,离线地图没有实时路况
    if (showTrafficControl && !mapLocal) {
        list << QString("  var ctrlTra = new %1Lib.TrafficControl({showPanel:false});").arg(mapFlag);
        list << QString("  ctrlTra.setAnchor(BMAP_ANCHOR_BOTTOM_RIGHT);");
        list << QString("  map.addControl(ctrlTra);");
    }

    //设置颜色主题,2019-6开始收费,屏蔽下面这行即可
    //2022-3再次测试这个接口发现又可以了
    if (!mapStyleName.isEmpty()) {
        list << QString("  map.setMapStyle({style:'%1'});").arg(mapStyleName);
    }

    //设置鼠标单击获取经度纬度,通过信号发出去
    if (enableClickPoint) {
        list << QString("  map.addEventListener('click', function(e) {");
        list << QString("    var point = e.point.lng + ',' + e.point.lat;");
        list << QString("    receiveData('point', point);");
        list << QString("  });");
    }
#if 0
    //部分设备需要通过事件机制实现鼠标滚轮缩放
    if (enableScrollWheelZoom) {
        list << QString("  window.addEventListener('mousewheel', function(e) {");
        list << QString("    var wheel = e.wheelDelta;");
        list << QString("    if (wheel > 0) {");
        list << QString("      map.zoomIn();");
        list << QString("    } else {");
        list << QString("      map.zoomOut();");
        list << QString("    }");
        list << QString("  });");
    }
#endif
}

void MapBaiDu::addFunction(QStringList &list)
{
    if (!addFun) {
        return;
    }

    getPoints(list);
    getMarkers(list);
    getPathPoints(list);
    getBounds(list);
    getBoundary(list);
    getZoom(list);

    addOther(list);
    addGeocoder(list);
    addLocalSearch(list);
    addClick(list);

    addMarker(list);
    moveMarker(list);
    deleteMarker(list);
    deleteOverlay(list);
    getOverlayInfo(list);

    getProperty(list);
    addPolyline(list);
    addPolygon(list);
    addRectangle(list);
    addCircle(list);
    addCurveLine(list);

    addBoundary(list);
    addMarkerClusterer(list);
    addPointCollection(list);
}

void MapBaiDu::getPoints(QStringList &list)
{
    //坐标字符串转坐标对象
    list << QString("  function getPoint(point) {");
    list << QString("    var list = point.split(',');");
    list << QString("    var pt = new %1.Point(list[0], list[1]);").arg(mapFlag);
    list << QString("    return pt;");
    list << QString("  }");

    //通用多个点坐标转换成 Point 对象
    //坐标格式 121.414,31.1828|121.414,31.1838|121.416,31.1838
    list << QString("  function getPoints(points) {");
    list << QString("    var pts = []");
    list << QString("    var listPoint = points.split('|');");
    list << QString("    var count = listPoint.length");
    list << QString("    for (var i = 0; i < count; ++i) {");
    list << QString("      pts.push(getPoint(listPoint[i]));");
    list << QString("    }");
    list << QString("    return pts;");
    list << QString("  }");
}

void MapBaiDu::getMarkers(QStringList &list)
{
    //通用多个点坐标转换成 Marker 对象
    //坐标格式 121.414,31.1828|121.414,31.1838|121.416,31.1838
    list << QString("  function getMarkers(points) {");
    list << QString("    var markers = []");
    list << QString("    var listPoint = points.split('|');");
    list << QString("    var count = listPoint.length");
    list << QString("    for (var i = 0; i < count; ++i) {");
    list << QString("      var marker = new %1.Marker(getPoint(listPoint[i]));").arg(mapFlag);
    list << QString("      markers.push(marker);");
    list << QString("    }");
    list << QString("    return markers;");
    list << QString("  }");
}

void MapBaiDu::getPathPoints(QStringList &list)
{
    list << QString("  function getPathPoints(path) {");
    list << QString("    var points = [];");
    list << QString("    var count = path.length;");
    list << QString("    for (var i = 0; i < count; ++i) {");
    list << QString("      var point = path[i].lng + ',' + path[i].lat;");
    list << QString("      points.push(point);");
    list << QString("    }");
    list << QString("    return points;");
    list << QString("  }");
}

void MapBaiDu::getBounds(QStringList &list)
{
    //获取可视区域经纬度 bssw=左下角 bsne=右上角 bsce=中心
    list << QString("  function getBounds() {");
    list << QString("    var bs = map.getBounds();");
    list << QString("    var bssw = bs.getSouthWest();");
    list << QString("    var bsne = bs.getNorthEast();");
    list << QString("    var bsce = bs.getCenter();");
    list << QString("    var bounds = bssw.lng + ',' + bssw.lat + ',' + bsne.lng + ',' + bsne.lat + ',' + bsce.lng + ',' + bsce.lat + ',' + map.getZoom();");
    //信号发出去
    list << QString("    receiveData('bounds', bounds);");
    list << QString("  }");
}

void MapBaiDu::getBoundary(QStringList &list)
{
    //获取边界点集合
    list << QString("  function getBoundary() {");
    list << QString("    var count = polygons.length;");
    list << QString("    if (count <= 0) {");
    list << QString("      return;");
    list << QString("    }");

    //多条边界点分别存入
    list << QString("    var pointsAll = [];");
    list << QString("    for (var i = 0; i < count; ++i) {");
    list << QString("      var path = polygons[i].getPath();");
    list << QString("      var points = getPathPoints(path);");
    list << QString("      pointsAll.push(points.join(';'));");
    list << QString("    }");
    list << QString("    receiveData('newboundary', pointsAll.join('|'));");
    list << QString("  }");
}

void MapBaiDu::getZoom(QStringList &list)
{
    //获取地图缩放级别
    list << QString("  function getZoom() {");
    list << QString("    var zoom = map.getZoom();");
    list << QString("    receiveData('zoom', zoom);");
    list << QString("  }");

    //设置地图缩放级别
    list << QString("  function setZoom(zoom) {");
    list << QString("    map.setZoom(zoom);");
    list << QString("  }");
}

void MapBaiDu::addOther(QStringList &list)
{
    //启用或关闭测距
    list << QString("var distanceTool = new BMapLib.DistanceTool(map, {lineStroke:2});");
    list << QString("  function setDistance(open) {");
    list << QString("    if (open) {");
    list << QString("      distanceTool.open();");
    list << QString("    } else {");
    list << QString("      distanceTool.close();");
    list << QString("    }");
    list << QString("  }");

    //设置地图中心点
    list << QString("  function setCenter(point) {");
    list << QString("    map.setCenter(getPoint(point));");
    list << QString("  }");

    //设置地图样式
    list << QString("  function setMapStyle(style) {");
    list << QString("    map.setMapStyle({style:style});");
    list << QString("  }");

    //设置地图类型 街道图: BMAP_NORMAL_MAP 卫星图: BMAP_SATELLITE_MAP 混合图: BMAP_HYBRID_MAP
    list << QString("  function setMapType(type) {");
    list << QString("    if (type == 1) {");
    list << QString("      map.setMapType(BMAP_SATELLITE_MAP);");
    list << QString("    } else if (type == 2) {");
    list << QString("      map.setMapType(BMAP_HYBRID_MAP);");
    list << QString("    } else {");
    list << QString("      map.setMapType(BMAP_NORMAL_MAP);");
    list << QString("    }");
    list << QString("  }");

    /**
     * COORDINATES_WGS84 = 1,       WGS84坐标 一般也叫GPS坐标
     * COORDINATES_WGS84_MC = 2,    WGS84的平面墨卡托坐标
     * COORDINATES_GCJ02 = 3,       GCJ02坐标 一般也叫谷歌坐标
     * COORDINATES_GCJ02_MC = 4,    GCJ02的平面墨卡托坐标
     * COORDINATES_BD09 = 5,        百度bd09经纬度坐标
     * COORDINATES_BD09_MC = 6,     百度bd09墨卡托坐标
     * COORDINATES_MAPBAR = 7,      mapbar地图坐标  测试发现没用
     * COORDINATES_51 = 8,          51地图坐标      测试发现没用
    */
    //坐标转换
    list << QString("  var convert = new %1.Convertor();").arg(mapFlag);
    list << QString("  function convertor(point, type1, type2) {");
    list << QString("    var points = [];");
    list << QString("    if (undefined == type1 || type1 < 1 || type1 > 6) {");
    list << QString("      type1 = 1;");
    list << QString("    }");
    list << QString("    if (undefined == type2 || type1 < 1 || type1 > 6) {");
    list << QString("      type2 = 5;");
    list << QString("    }");
    list << QString("    points.push(getPoint(point));");
    list << QString("    convert.translate(points, type1, type2, function(data) {");
    list << QString("      if (data.status === 0) {");
    list << QString("        var pt = data.points[0].lng + ',' + data.points[0].lat;");
    list << QString("        receiveData('convertor', pt);");
    list << QString("      }");
    list << QString("    })");
    list << QString("  }");
}

void MapBaiDu::addGeocoder(QStringList &list)
{
    list << QString("  var geo = new %1.Geocoder();").arg(mapFlag);
    //地址解析成坐标
    list << QString("  function getPointByAddr(addr) {");
    list << QString("    geo.getPoint(addr, function(result) {");
    list << QString("      if (result) {");
    list << QString("        var point = result.lng + ',' + result.lat;");
    list << QString("        receiveData('geocoder', point);");
    list << QString("      }");
    list << QString("    });");
    list << QString("  }");
    //坐标解析成地址
    list << QString("  function getAddrByPoint(point) {");
    list << QString("    geo.getLocation(getPoint(point), function(result) {");
    list << QString("      if (result) {");
    list << QString("        receiveData('geocoder', result.address);");
    list << QString("      }");
    list << QString("    });");
    list << QString("  }");
}

void MapBaiDu::addLocalSearch(QStringList &list)
{
    //全局搜索对象 结果可选显示在地图中还是异步通知
#if 1
    list << QString("  var searchOptions = {renderOptions:{map: map}};");
#else
    list << QString("  var searchOptions = {");
    list << QString("    onSearchComplete: function(results) {");
    list << QString("      if (local.getStatus() != BMAP_STATUS_SUCCESS) {");
    list << QString("        return;");
    list << QString("      }");
    list << QString("      var result = [];");
    list << QString("      var count = results.getCurrentNumPois();");
    list << QString("      for (var i = 0; i < count; ++i) {");
    list << QString("        var poi = results.getPoi(i);");
    list << QString("        var point = poi.point.lng + ',' + poi.point.lat;");
    list << QString("        result.push(point + ';' + poi.title + ';' + poi.address + ';' + poi.url);");
    list << QString("      }");
    list << QString("      receiveData('searchresult', result.join('|'));");
    list << QString("    }");
    list << QString("  };");
#endif
    list << QString("  var local = new %1.LocalSearch(map, searchOptions);").arg(mapFlag);

    list << QString("  local.setSearchCompleteCallback(function(results) {");
    list << QString("    ");
    list << QString("  });");

    //通用搜索方法
    //text: 搜索的关键字 支持多个可以用 | 隔开
    list << QString("  function search(text) {");
    list << QString("    var texts = text.split('|');");
    list << QString("    local.search(texts);");
    list << QString("  }");

    //矩形区域搜索
    list << QString("  function searchInBounds(text, start, end) {");
    list << QString("    var texts = text.split('|');");
    list << QString("    if (!start || !end) {");
    list << QString("      local.searchInBounds(texts, map.getBounds());");
    list << QString("      return;");
    list << QString("    }");
    list << QString("    var bs = new %1.Bounds(getPoint(start), getPoint(end));").arg(mapFlag);
    list << QString("    local.searchInBounds(texts, bs);");
    list << QString("  }");

    //圆形区域搜索
    list << QString("  function searchNearby(text, center, radius) {");
    list << QString("    var texts = text.split('|');");
    list << QString("    local.searchNearby(texts, getPoint(center), radius);");
    list << QString("  }");
}

void MapBaiDu::addClick(QStringList &list)
{
    //自定义方法显示标注详细信息,可以改成自己的内容,标准html格式

    //默认格式如下,在传入的信息为空的时候应用
    QString title = "<div style=\"color:#CE5521;font-size:15px;\">' + name + '</div>";
    QStringList tips;
    tips << "<table><tr style=\"vertical-align:top;line-height:25px;font-size:12px;\">";
    tips << "<td>地址：</td><td>' + addr + '</td>";
    tips << "</tr></table>";

    list << QString("  function addClick(marker, name, addr, title, tips, width, action){");
    list << QString("    if (title == '' && name != '') {");
    list << QString("      title = '%1';").arg(title);
    list << QString("    }");
    list << QString("    if (tips == '' && addr != '') {");
    list << QString("      tips = '%1';").arg(tips.join(""));
    list << QString("    }");
    list << QString("    var infoWindow = new %1.InfoWindow(tips, {title:title, width:width});").arg(mapFlag);

    //单击以后弹出提示信息或者发送信号出去
    list << QString("    var markerClick = function() {");
    list << QString("      if (action == 1) {");
    list << QString("        marker.openInfoWindow(infoWindow);");
    list << QString("      } else if (action == 2) {");
    list << QString("        var info = '名称: ' + name + '  地址: ' + addr;");
    list << QString("        receiveData('marker', info);");
    list << QString("      }");
    list << QString("    };");

    //添加单击监听器
    list << QString("    marker.addEventListener('click', markerClick);");
    list << QString("  }");
}

void MapBaiDu::addMarker(QStringList &list)
{
    //动态添加点
    //name      表示标注点名称,显示在图标旁边的文本,为空则不显示
    //addr      表示标注点地址
    //title     表示弹框信息html格式标题
    //tips      表示弹框信息html格式内容
    //width     表示弹框的宽度
    //point     表示经纬度坐标
    //action    表示单击以后触发什么动作 0-不处理 1-自己弹框 2-发送信号
    //animation 表示动画效果 0-不处理 1-跳动 2-坠落
    //iconfile  表示图标文件路径,不设置则采用默认图标,注意图片的尺寸
    //iconindex 表示图标对应在图片中的索引,本地文件则当做图片尺寸
    //rotation  表示图标旋转角度
    list << QString("  function addMarker(name, addr, title, tips, width, point, action, animation, iconfile, iconindex, rotation) {");
    //设置点经纬度坐标
    list << QString("    var pot = getPoint(point);");
    //设置文本文字 offset为对应标签显示的位置偏移值
    list << QString("    var label = new %1.Label(name, {\"offset\":new %1.Size(20, -10)});").arg(mapFlag);
    //下面演示的通过换行和偏移值来实现垂直展示文字
    //list << QString("    var label = new %1.Label('8.<br/>测<br/>试<br/>文<br/>字', {'offset':new %1.Size(0, -80)});").arg(mapFlag);

    //设置图标,不设置则采用默认图标
    list << QString("    if (!iconfile) {");
    list << QString("      var marker = new %1.Marker(pot);").arg(mapFlag);
    list << QString("    } else if (iconfile == 'http://lbsyun.baidu.com/jsdemo/img/fox.gif') {");
    list << QString("      var icon = new %1.Icon(iconfile, new %1.Size(300, 157));").arg(mapFlag);
    list << QString("      var marker = new %1.Marker(pot, {icon: icon, rotation: rotation});").arg(mapFlag);
    list << QString("    } else if (iconfile == 'http://api.map.baidu.com/img/markers.png') {");
    list << QString("      var icon = new %1.Icon(iconfile, new %1.Size(23, 25), {offset: new %1.Size(10, 25), imageOffset: new %1.Size(0, 0 - iconindex * 25)});").arg(mapFlag);
    list << QString("      var marker = new %1.Marker(pot, {icon: icon, rotation: rotation});").arg(mapFlag);
    list << QString("    } else {");
    list << QString("      var size = 50;");
    list << QString("      if (iconindex) {");
    list << QString("        size = iconindex;");
    list << QString("      }");
    list << QString("      label = new %1.Label(name, {\"offset\":new %1.Size(size, -10)});").arg(mapFlag);
    list << QString("      var icon = new %1.Icon(iconfile, new %1.Size(size, size));").arg(mapFlag);
    list << QString("      var marker = new %1.Marker(pot, {icon: icon, rotation: rotation});").arg(mapFlag);
    list << QString("    }");

    list << QString("    map.addOverlay(marker);");
    list << QString("    if (name != '') {");
    list << QString("      marker.setLabel(label);");
    list << QString("    }");
    list << QString("    addClick(marker, name, addr, title, tips, width, action);");

    //弹跳效果-BMAP_ANIMATION_BOUNCE 坠落效果-BMAP_ANIMATION_DROP
    list << QString("    if (animation == 1) {");
    list << QString("      marker.setAnimation(BMAP_ANIMATION_BOUNCE);");
    list << QString("    } else if (animation == 2) {");
    list << QString("      marker.setAnimation(BMAP_ANIMATION_DROP);");
    list << QString("    }");

    list << QString("  }");
}

void MapBaiDu::moveMarker(QStringList &list)
{
    //动态移动点
    list << QString("  function moveMarker(name, point, rotation) {");
    list << QString("    if (name.length == 0) {");
    list << QString("      return;");
    list << QString("    }");
    list << QString("    var allOverlay = map.getOverlays();");
    list << QString("    var count = allOverlay.length;");
    list << QString("    for (var i = 0; i < count; ++i) {");
    list << QString("      var overlay = allOverlay[i];");
    //过滤只需要标注点 Marker 的图层覆盖物
    list << QString("      if (overlay.toString() != '[object Marker]') {");
    list << QString("        continue;");
    list << QString("      }");
    //过滤没有标签的标注点
    list << QString("      var label = overlay.getLabel();");
    list << QString("      if (label == null) {");
    list << QString("        continue;");
    list << QString("      }");
    list << QString("      if (label.content == name) {");
    list << QString("        var marker = allOverlay[i];");
    list << QString("        marker.setPosition(getPoint(point));");
    list << QString("        marker.setRotation(rotation);");
    list << QString("        break;");
    list << QString("      }");
    list << QString("    }");
    list << QString("  }");
}

void MapBaiDu::deleteMarker(QStringList &list)
{
    //动态删除点
    list << QString("  function deleteMarker(name) {");
    list << QString("    var allOverlay = map.getOverlays();");
    list << QString("    var count = allOverlay.length;");
    list << QString("    for (var i = 0; i < count; ++i) {");
    list << QString("      var overlay = allOverlay[i];");
    //过滤只需要标注点 Marker 的图层覆盖物
    list << QString("      if (overlay.toString() != '[object Marker]') {");
    list << QString("        continue;");
    list << QString("      }");
    //名字为空则表示删除所有
    list << QString("      if (name.length == 0) {");
    list << QString("        map.removeOverlay(overlay);");
    list << QString("      } else {");
    //过滤没有标签的标注点
    list << QString("        var label = overlay.getLabel();");
    list << QString("        if (label == null) {");
    list << QString("          continue;");
    list << QString("        }");
    list << QString("        if (label.content == name) {");
    list << QString("          map.removeOverlay(allOverlay[i]);");
    list << QString("          break;");
    list << QString("        }");
    list << QString("      }");
    list << QString("    }");
    list << QString("  }");
}

void MapBaiDu::deleteOverlay(QStringList &list)
{
    //动态删除覆盖物 type = Marker Polyline 等
    list << QString("  function deleteOverlay(type) {");
    //如果为空还要先清空点聚合的数据
    list << QString("    if (type.length == 0) {");
    list << QString("      markerClusterer.clearMarkers();");
    list << QString("    }");
    //取出所有覆盖物
    list << QString("    var allOverlay = map.getOverlays();");
    list << QString("    var count = allOverlay.length;");
    list << QString("    for (var i = 0; i < count; ++i) {");
    list << QString("      var overlay = allOverlay[i];");
    //拿到覆盖物类型对应字符串 toString() 以后的值是 [object Marker] [object Circle] [object Overlay]
    list << QString("      var objType = overlay.toString();");
    list << QString("      objType = objType.substr(8, objType.length - 9);");
    //如果指定了覆盖物类型则过滤掉不符合类型的
    list << QString("      if (type.length > 0 && objType != type) {");
    list << QString("        continue;");
    list << QString("      }");
    list << QString("      map.removeOverlay(overlay);");
    list << QString("    }");
    list << QString("  }");
}

void MapBaiDu::getOverlayInfo(QStringList &list)
{
    //获取覆盖物信息 type = Marker Polyline 等
    list << QString("  function getOverlayInfo(type) {");
    list << QString("    var allOverlay = map.getOverlays();");
    list << QString("    var count = allOverlay.length;");
    list << QString("    for (var i = 0; i < count; ++i) {");
    list << QString("      var overlay = allOverlay[i];");
    //过滤不可见的
    list << QString("      if (!overlay.isVisible()) {");
    list << QString("        continue;");
    list << QString("      }");
    //拿到覆盖物类型对应字符串 toString() 以后的值是 [object Marker] [object Circle] [object Overlay]
    list << QString("      var objType = overlay.toString();");
    list << QString("      objType = objType.substr(8, objType.length - 9);");
    //过滤标注点和悬停工具
    list << QString("      if (objType == 'Marker' || objType == 'Overlay') {");
    list << QString("        continue;");
    list << QString("      }");
    //如果指定了覆盖物类型则过滤掉不符合类型的
    list << QString("      if (type.length > 0 && objType != type) {");
    list << QString("        continue;");
    list << QString("      }");
    list << QString("      var path = overlay.getPath();");
    list << QString("      var points = getPathPoints(path);");
    //不同类型的有不同的信息 下面演示的是另外一种过滤方法 instanceof
    list << QString("      var info = objType;");
    list << QString("      if (overlay instanceof %1.Circle) {").arg(mapFlag);
    list << QString("        var center = overlay.getCenter();");
    list << QString("        var point = center.lng + ',' + center.lat;");
    list << QString("        var radius = overlay.getRadius();");
    list << QString("        info = objType + '|' + point + '|' + radius + '|' + points.join(';')");
    list << QString("      } else {");
    list << QString("        info = objType + '|' + points.join(';')");
    list << QString("      }");
    list << QString("      receiveData('overlayinfo', info);");
    list << QString("    }");
    list << QString("  }");
}

void MapBaiDu::getProperty(QStringList &list)
{
    //可以自行设置颜色等属性
    list << QString("  function getProperty(color, weight, opacity) {");
    list << QString("    var property = {strokeColor:color, strokeWeight:weight, strokeOpacity:opacity};");
    list << QString("    return property;");
    list << QString("  }");
}

void MapBaiDu::addPolyline(QStringList &list)
{
    //动态添加折线
    list << QString("  function addPolyline(points, color, weight, opacity) {");
    list << QString("    var pts = getPoints(points);");
    list << QString("    var property = getProperty(color, weight, opacity);");
    list << QString("    var polyline = new %1.Polyline(pts, property);").arg(mapFlag);
    list << QString("    map.addOverlay(polyline);");
    list << QString("  }");
}

void MapBaiDu::addPolygon(QStringList &list)
{
    //动态添加多边形
    list << QString("  function addPolygon(points, color, weight, opacity) {");
    list << QString("    var pts = getPoints(points);");
    list << QString("    var property = getProperty(color, weight, opacity);");
    list << QString("    var polygon = new %1.Polygon(pts, property);").arg(mapFlag);
    list << QString("    map.addOverlay(polygon);");
    list << QString("  }");
}

void MapBaiDu::addRectangle(QStringList &list)
{
    //动态添加矩形
    list << QString("  function addRectangle(points, color, weight, opacity) {");
    list << QString("    var listPoint = points.split('|');");
    list << QString("    if (listPoint.length != 2) {");
    list << QString("      return;");
    list << QString("    }");

    //将两个坐标拆分成四个点
    list << QString("    var ptStart = getPoint(listPoint[0]);");
    list << QString("    var ptEnd = getPoint(listPoint[1]);");
    list << QString("    var pt1 = new %1.Point(ptStart.lng, ptStart.lat);").arg(mapFlag);
    list << QString("    var pt2 = new %1.Point(ptEnd.lng, ptStart.lat);").arg(mapFlag);
    list << QString("    var pt3 = new %1.Point(ptEnd.lng, ptEnd.lat);").arg(mapFlag);
    list << QString("    var pt4 = new %1.Point(ptStart.lng, ptEnd.lat);").arg(mapFlag);
    list << QString("    var property = getProperty(color, weight, opacity);");
    list << QString("    var rectangle = new %1.Polygon([pt1,pt2,pt3,pt4], property);").arg(mapFlag);
    list << QString("    map.addOverlay(rectangle);");
    list << QString("  }");
}

void MapBaiDu::addCircle(QStringList &list)
{
    //动态添加圆形
    list << QString("  function addCircle(points, radius, color, weight, opacity) {");
    list << QString("    var center = getPoint(listPoint[0]);");
    list << QString("    var property = getProperty(color, weight, opacity);");
    list << QString("    var circle = new %1.Circle(center, radius, property);").arg(mapFlag);
    list << QString("    map.addOverlay(circle);");
    list << QString("  }");
}

void MapBaiDu::addCurveLine(QStringList &list)
{
    //动态添加弧线
    list << QString("  function addCurveLine(points, color, weight, opacity) {");
    list << QString("    var pts = getPoints(points);");
    list << QString("    var property = getProperty(color, weight, opacity);");
    list << QString("    var curveLine = new %1Lib.CurveLine(pts, property);").arg(mapFlag);
    list << QString("    map.addOverlay(curveLine);");
    //弧线允许拖动
    list << QString("    curveLine.enableEditing();");
    list << QString("  }");
}

void MapBaiDu::addBoundary(QStringList &list)
{
    //定义数组存储可拖动的边界点
    //在这里定义,在getBoundary函数调用的
    list << QString("  var polygons = [];");

    //动态添加行政区划
    list << QString("  function addBoundary(cityname, callfun, edit, color, weight, opacity) {");
    //清除地图覆盖物
    list << QString("    map.clearOverlays();");

    //离线地图通过文件直接加载,上面已经引入了对应 cityname 的js文件
    //通过读取js数组数据直接绘制不规则形状
    if (mapLocal) {
        QString property = getOverlayProperty();
        list << QString("    var pointArray = [];");
        list << QString("    var count = boundarys.length;");
        list << QString("    for (var i = 0; i < count; ++i) {");
        list << QString("      var ply = new %1.Polygon(boundarys[i].points, %2);").arg(mapFlag).arg(property);
        list << QString("      map.addOverlay(ply);");
        list << QString("      pointArray = pointArray.concat(ply.getPath());");
        list << QString("    }");
        //调整视野自适应行政区划区域
        list << QString("    map.setViewport(pointArray);");
        list << QString("  }");
        return;
    }

    //清除之前存储的边界点数组数据
    list << QString("    polygons = [];");
    //实例化行政区划对象
    list << QString("    var bdary = new %1.Boundary();").arg(mapFlag);
    //调用内置的方法获取城市的点集合
    list << QString("    bdary.get(cityname, function(rs) {");
    //行政区域的点有多少个
    list << QString("      var datas = rs.boundaries;");
    list << QString("      var count = datas.length;");
    list << QString("      if (count <= 0) {");
    list << QString("        return;");
    list << QString("      }");

    list << QString("      var pointsAll = [];");
    list << QString("      var pointArray = [];");
    list << QString("      for (var i = 0; i < count; ++i) {");
    //建立多边形覆盖物
    list << QString("        var property = getProperty(color, weight, opacity);");
    //建立多边形覆盖物
    list << QString("        var ply = new %1.Polygon(datas[i], property);").arg(mapFlag);
    //取出点集合
    list << QString("        var path = ply.getPath();");
    list << QString("        var points = getPathPoints(path);");
    list << QString("        pointsAll.push(points.join(';'));");
    //范围可编辑,开启以后可以拖动边界然后可以重新输出边界点集合
    list << QString("        if (edit) {");
    list << QString("          ply.enableEditing();");
    list << QString("        }");
    //添加到多边形数组,用来后边获取调整后的边界数组
    list << QString("        polygons.push(ply);");
    //添加覆盖物
    list << QString("        map.addOverlay(ply);");
    list << QString("        pointArray = pointArray.concat(path);");
    list << QString("      }");
    //调整视野自适应行政区划区域
    list << QString("      map.setViewport(pointArray);");

    //弹出+调试输出+回调输出 行政区划的边界坐标点集合
    //list << QString("      alert(count);");
    //list << QString("      alert(datas);");
    //list << QString("      console.log(datas);");
    list << QString("      if (callfun) {");
    list << QString("        receiveData('boundary', pointsAll.join('|'));");
    list << QString("      }");
    list << QString("    });");
    list << QString("  }");
}

void MapBaiDu::addMarkerClusterer(QStringList &list)
{
    //定义成全局的对象,方便清空以及每次只需要添加对象就行
    list << QString("  var markerClusterer = new %1Lib.MarkerClusterer(map);").arg(mapFlag);
    //动态添加点聚合
    list << QString("  function addMarkerClusterer(points) {");
    //清空原有的数据
    list << QString("    markerClusterer.clearMarkers();");
    //转换成marker数组
    list << QString("    var markers = getMarkers(points);");
    //添加到点聚合对象中
    list << QString("    markerClusterer.addMarkers(markers);");
    list << QString("  }");
}

void MapBaiDu::addPointCollection(QStringList &list)
{
    //动态添加海量点
    list << QString("  function addPointCollection(points, color, size, shape) {");
    //样式参数 https://blog.csdn.net/hmyhicc/article/details/84661436
    list << QString("    var options = {");
    list << QString("      color: color,");
    //1 = BMAP_POINT_SIZE_TINY      2px*2px
    //2 = BMAP_POINT_SIZE_SMALLER   4px*4px
    //3 = BMAP_POINT_SIZE_SMALL     8px*8px
    //4 = BMAP_POINT_SIZE_NORMAL    10px*10px 默认值
    //5 = BMAP_POINT_SIZE_BIG       16px*16px
    //6 = BMAP_POINT_SIZE_BIGGER    20px*20px
    //7 = BMAP_POINT_SIZE_HUGE      30px*30px
    list << QString("      size: size,");
    //1 = BMAP_POINT_SHAPE_CIRCLE   圆形 默认值
    //2 = BMAP_POINT_SHAPE_STAR     星形
    //3 = BMAP_POINT_SHAPE_SQUARE   方形
    //4 = BMAP_POINT_SHAPE_RHOMBUS  菱形
    //5 = BMAP_POINT_SHAPE_WATERDROP水滴
    list << QString("      shape: shape");
    list << QString("    }");

    //先转换成经纬度坐标点数组
    list << QString("    var pts = getPoints(points);");
    list << QString("    var pointCollection = new %1.PointCollection(pts, options);").arg(mapFlag);
    //关联单击点事件
    list << QString("    pointCollection.addEventListener('click', function (e) {");
    list << QString("      var point = e.point.lng + ',' + e.point.lat;");
    list << QString("      receiveData('pointcollection', point);");
    list << QString("    });");
    list << QString("    map.addOverlay(pointCollection);");
    list << QString("  }");
}

void MapBaiDu::addDevice(QStringList &list)
{
    if (markerNames.count() != markerAddrs.count() || markerAddrs.count() != markerPoints.count()) {
        return;
    }

    //添加多个标记点及提示信息
    int count = markerNames.count();
    if (count > 0) {
        //构建标记信息数组
        list << QString("  var markers = [");
        QStringList markers;
        for (int i = 0; i < count; ++i) {
            markers << QString("    {name:\"%1\", addr:\"%2\", point:\"%3\"}")
                    .arg(markerNames.at(i)).arg(markerAddrs.at(i)).arg(markerPoints.at(i));
        }

        list << markers.join(",\r\n");
        list << QString("  ];");

        //循环添加标注
        list << QString("  var count = markers.length;");
        list << QString("  for (var i = 0; i < count; ++i) {");
        list << QString("    var iconfile = 'http://api.map.baidu.com/img/markers.png';");
        list << QString("    addMarker(markers[i].name, markers[i].addr, '', '', 60, markers[i].point, 2, 0, iconfile, i);");
        list << QString("  }");
    }
}

void MapBaiDu::addOverlayTool(QStringList &list)
{
    //设置点线面工具
    if (showOverlayTool) {
        list << QString("  var overlays = [];");
        list << QString("  var overlaycomplete = function(e) {");
        list << QString("    overlays.push(e.overlay);");
        list << QString("  };");

        list << QString("  var styleOptions = {");
        //边线颜色
        list << QString("    strokeColor:\"red\",");
        //填充颜色,当参数为空时圆形将没有填充效果
        list << QString("    fillColor:\"red\",");
        //边线的宽度,以像素为单位
        list << QString("    strokeWeight:3,");
        //边线透明度,取值范围0 - 1
        list << QString("    strokeOpacity:0.8,");
        //填充的透明度,取值范围0 - 1
        list << QString("    fillOpacity:0.6,");
        //边线的样式,solid或dashed
        list << QString("    strokeStyle:'solid'");
        list << QString("  }");

        //实例化鼠标绘制工具
        list << QString("  var drawingManager = new %1Lib.DrawingManager(map, {").arg(mapFlag);
        //是否开启绘制模式
        list << QString("    isOpen:false,");
        //是否显示工具栏
        list << QString("    enableDrawingTool:true,");
        //位置+//偏离值
        list << QString("    drawingToolOptions:{anchor:BMAP_ANCHOR_TOP_RIGHT, offset:new %1.Size(100, 5)},").arg(mapFlag);
        //圆的样式+线的样式+多边形的样式+矩形的样式
        list << QString("    circleOptions:styleOptions, polylineOptions:styleOptions, polygonOptions:styleOptions, rectangleOptions:styleOptions");
        list << QString("  });");

        //添加鼠标绘制工具监听事件,用于获取绘制结果
        list << QString("  drawingManager.addEventListener('overlaycomplete', overlaycomplete);");

        //添加清空函数用于交互
        list << QString("  function clearAll() {");
        list << QString("    var count = overlays.length;");
        list << QString("    for(var i = 0; i < count; ++i) {");
        list << QString("      map.removeOverlay(overlays[i]);");
        list << QString("    }");
        list << QString("    overlays.length = 0;");
        list << QString("  }");
    }
}

QString MapBaiDu::getOverlayProperty()
{
    //可以自行设置颜色等属性
    QString property = QString("{strokeColor:\"%1\", strokeWeight:%2, strokeOpacity:%3}").arg(strokeColor).arg(strokeWeight).arg(strokeOpacity);
    return property;
}

void MapBaiDu::addOverlay(QStringList &list)
{
    if (overlayTypes.count() == 0 || overlayTypes.count() != overlayPoints.count()) {
        return;
    }

    //判断不同的类型,添加多个覆盖物
    int count = overlayTypes.count();
    for (int i = 0; i < count; ++i) {
        OverlayType type = overlayTypes.at(i);
        QStringList points = overlayPoints.at(i);
        QString property = getOverlayProperty();

        //为了支持多个覆盖物,每个覆盖物对应的变量名都不一样
        QString name;
        if (type == OverlayType_Marker) {
            name = QString("marker%1").arg(i + 1);
            list << QString("  var %2 = new %1.Marker(new %1.Point(%3));").arg(mapFlag).arg(name).arg(points.at(0));
        } else if (type == OverlayType_Polyline) {
            //每个图形都有点集合
            QStringList listPoints;
            foreach (QString point, points) {
                listPoints << QString("new %1.Point(%2)").arg(mapFlag).arg(point);
            }

            name = QString("polyline%1").arg(i + 1);
            list << QString("  var %2 = new %1.Polyline([%3], %4);").arg(mapFlag).arg(name).arg(listPoints.join(",")).arg(property);
        } else if (type == OverlayType_Polygon) {
            //每个图形都有点集合
            QStringList listPoints;
            foreach (QString point, points) {
                listPoints << QString("new %1.Point(%2)").arg(mapFlag).arg(point);
            }

            name = QString("polygon%1").arg(i + 1);
            list << QString("  var %2 = new %1.Polygon([%3], %4);").arg(mapFlag).arg(name).arg(listPoints.join(",")).arg(property);
        } else if (type == OverlayType_Rectangle) {
            if (points.count() < 2) {
                continue;
            }

            name = QString("rectangle%1").arg(i + 1);
            QString ptStart = QString("pStart%1").arg(i + 1);
            QString ptEnd = QString("pEnd%1").arg(i + 1);

            list << QString("  var %2 = new %1.Point(%3);").arg(mapFlag).arg(ptStart).arg(points.at(0));
            list << QString("  var %2 = new %1.Point(%3);").arg(mapFlag).arg(ptEnd).arg(points.at(1));
            list << QString("  var %2 = new %1.Polygon([").arg(mapFlag).arg(name);
            list << QString("    new %1.Point(%2.lng, %3.lat),").arg(mapFlag).arg(ptStart).arg(ptStart);
            list << QString("    new %1.Point(%2.lng, %3.lat),").arg(mapFlag).arg(ptEnd).arg(ptStart);
            list << QString("    new %1.Point(%2.lng, %3.lat),").arg(mapFlag).arg(ptEnd).arg(ptEnd);
            list << QString("    new %1.Point(%2.lng, %3.lat)").arg(mapFlag).arg(ptStart).arg(ptEnd);
            list << QString("  ], %1);").arg(property);
        } else if (type == OverlayType_Circle) {
            if (points.count() < 2) {
                continue;
            }

            //取第二个值作为半径
            int circleRadius = points.at(1).toInt();
            name = QString("circle%1").arg(i + 1);
            list << QString("  var point%2 = new %1.Point(%3);").arg(mapFlag).arg(i + 1).arg(points.at(0));
            list << QString("  var %2 = new %1.Circle(point%3, %4, %5);").arg(mapFlag).arg(name).arg(i + 1).arg(circleRadius).arg(property);
        } else if (type == OverlayType_CurveLine) {
            //弧形点集合
            QStringList listPoints;
            foreach (QString point, points) {
                listPoints << QString("new %1.Point(%2)").arg(mapFlag).arg(point);
            }

            name = QString("curveline%1").arg(i + 1);
            list << QString("  var points%1 = [%2];").arg(i + 1).arg(listPoints.join(","));
            list << QString("  var %2 = new %1Lib.CurveLine(points%3, %4);").arg(mapFlag).arg(name).arg(i + 1).arg(property);
        } else if (type == OverlayType_Boundary) {
            //离线地图从本地加载行政区划边界点集合
            if (mapLocal) {
                //可能会有多个数据,比如台湾的金门岛,都是独立的区域形状
                //这个 boundarys 通过最上面js文件引入产生的变量
                list << QString("  var pointArray = [];");
                list << QString("  var count = boundarys.length;");
                list << QString("  for (var i = 0; i < count; ++i) {");
                list << QString("    var ply = new %1.Polygon(boundarys[i].points, %2);").arg(mapFlag).arg(property);
                list << QString("    map.addOverlay(ply);");
                list << QString("    pointArray = pointArray.concat(ply.getPath());");
                list << QString("  }");
                //调整视野自适应行政区划区域
                list << QString("  map.setViewport(pointArray);");
            } else {
                list << QString("  var bdary = new %1.Boundary();").arg(mapFlag);
                //获取行政区域
                list << QString("  bdary.get(\"%1\", function(rs) {").arg(points.first());
                //清除地图覆盖物
                list << QString("    map.clearOverlays();");
                //行政区域的点有多少个
                list << QString("    var datas = rs.boundaries;");
                list << QString("    var count = datas.length;");
                list << QString("    if (count <= 0) {");
                list << QString("      return;");
                list << QString("    }");

                list << QString("    var pointsAll = [];");
                list << QString("    var pointArray = [];");
                list << QString("    for (var i = 0; i < count; ++i) {");
                //建立多边形覆盖物
                list << QString("      var ply = new %1.Polygon(datas[i], %2);").arg(mapFlag).arg(property);
                //范围可编辑,开启以后可以拖动边界然后可以重新输出边界点集合
                //list << QString("      ply.enableEditing();");
                //取出点集合
                list << QString("      var path = ply.getPath();");
                list << QString("      var points = getPathPoints(path);");
                list << QString("      pointsAll.push(points.join(';'));");
                //添加覆盖物
                list << QString("      map.addOverlay(ply);");
                list << QString("      pointArray = pointArray.concat(path);");
                list << QString("    }");
                //调整视野自适应行政区划区域
                list << QString("    map.setViewport(pointArray);");

                //弹出+调试输出+回调输出 行政区划的边界坐标点集合
                //list << QString("    alert(count);");
                //list << QString("    alert(datas);");
                //list << QString("    console.log(datas);");
                list << QString("    receiveData('boundary', pointsAll.join('|'));");
                list << QString("  });");
            }
        } else if (type == OverlayType_MarkerClusterer) {
            //点集合
            QStringList listPoints;
            foreach (QString point, points) {
                listPoints << QString("new %1.Marker(new %1.Point(%2))").arg(mapFlag).arg(point);
            }

            list << QString("  var markers = [%1];").arg(listPoints.join(","));
            //最简单的用法,生成一个marker数组,然后调用markerClusterer类即可
            list << QString("  var markerClusterer = new %1Lib.MarkerClusterer(map, {markers:markers});").arg(mapFlag);
        }

        //添加覆盖物,行政区划的覆盖物已经在上面代码中添加
        if (!name.isEmpty()) {
            list << QString("  map.addOverlay(%1);").arg(name);
        }

        //设置其他效果,要放在addOverlay方法后面才有作用
        if (type == OverlayType_Marker) {
            //设置跳动的点动画
            list << QString("  %1.setAnimation(BMAP_ANIMATION_BOUNCE);").arg(name);
        } else if (type == OverlayType_CurveLine) {
            //开启编辑功能
            list << QString("  %1.enableEditing();").arg(name);
        }
    }
}

void MapBaiDu::addRoute(QStringList &list)
{
    if (startAddr.isEmpty() || endAddr.isEmpty()) {
        return;
    }

    //地址中带了 , 表示采用的经纬度形式
    if (startAddr.contains(",")) {
        list << QString("  var p1 = getPoint('%1');").arg(startAddr);
        list << QString("  var p2 = getPoint('%1');").arg(endAddr);
    } else {
        list << QString("  var p1 = \"%1\";").arg(startAddr);
        list << QString("  var p2 = \"%1\";").arg(endAddr);
    }

    //0-公交 1-驾车 2-步行 3-骑行
    QString renderOptions = QString("{renderOptions:{map:map, panel:\"result\", autoViewport:true}, policy:%1}").arg(policyType);
    if (routeType == 0) {
        list << QString("  var route = new %1.TransitRoute(map, %2);").arg(mapFlag).arg(renderOptions);
    } else if (routeType == 1) {
        list << QString("  var route = new %1.DrivingRoute(map, %2);").arg(mapFlag).arg(renderOptions);
    } else if (routeType == 2) {
        list << QString("  var route = new %1.WalkingRoute(map, %2);").arg(mapFlag).arg(renderOptions);
    } else if (routeType == 3) {
        list << QString("  var route = new %1.RidingRoute(map, %2);").arg(mapFlag).arg(renderOptions);
    }

    //获取路径的系列点
    QStringList temp;
    temp << QString("  route.setSearchCompleteCallback(function(results) {");
    temp << QString("    if (route.getStatus() != BMAP_STATUS_SUCCESS) {");
    temp << QString("      return;");
    temp << QString("    }");
    //获取路线结果,可能有多条,默认取第一条一般是最优的
    temp << QString("    var plan = results.getPlan(0);");
    //获取总时长
    temp << QString("    var duration = plan.getDuration(true);");
    //获取总路程
    temp << QString("    var distance = plan.getDistance(true);");
    //获取线路段数,可能有多段
    temp << QString("    var count = plan.getNumRoutes(0);");
    //temp << QString("    alert(count + '|' + duration + '|' + distance);");

    //获取所有坐标点位的数组
    temp << QString("    var pointsAll = [];");
    temp << QString("    for (var i = 0; i < count; ++i) {");
    temp << QString("      var path = plan.getRoute(i).getPath();");
    temp << QString("      var points = getPathPoints(path);");
    temp << QString("      pointsAll.push(points.join(';'));");
    temp << QString("    }");

    //返回总里程总用时结果
    temp << QString("    receiveData('routeresult', duration + '|' + distance);");
    //返回坐标点集合,多个用 | 隔开
    temp << QString("    receiveData('routepoints', pointsAll.join('|'));");
    temp << QString("  })");
    list << temp.join("\r\n");

    //查询路径
    list << QString("  route.search(p1, p2);");
}

void MapBaiDu::addEnd(QStringList &list)
{
    list << QString("</script>");
    list << QString("</html>");
}

void MapBaiDu::reset()
{
    height = 410;
    mapLocal = false;
    saveFile = true;
    addFun = true;

    mapFlag = "BMap";
    title = "百度地图(QQ: 517216493)";
    fileName = "map_baidu.html";
    cityJsName = "上海市徐汇区";

    //各种js函数参数解释 https://mapopen-pub-jsapi.bj.bcebos.com/jsapi/reference/jsapi_reference_3_0.html
    //1.5版本以上需要密钥 最新版本3.0 建议替换成自己的秘钥
    //mapVersionKey = "3.0&ak=9GqzBZjfGqXYXMzdfuRN9AL4B4PHaY6R";
    mapVersionKey = "3.0&ak=M3vTdD2f7ZjMFKold3TbOnEc6eYHx869";

    //normal-默认地图样式 light-清新蓝风格 googlelite-精简风格 hardedge-强边界
    //具体参见 http://developer.baidu.com/map/custom/stylelist.js
    mapStyleName = "";

    mapCenterPoint = "121.424362,31.175942";
    mapCenterCity = "上海";

    mapZoom = 12;
    mapMinZoom = 4;
    mapMaxZoom = 19;

    enableMapClick = false;
    enableDragging = true;
    enableScrollWheelZoom = true;
    enableDoubleClickZoom = true;
    enableKeyboard = false;
    enableAnimation = false;
    enableClickPoint = false;

    showNavigationControl = false;
    showOverviewMapControl = true;
    showScaleControl = false;
    showMapTypeControl = false;
    showPanoramaCoverageLayer = false;
    showTrafficControl = false;
    showOverlayTool = false;

    policyType = 0;
    startAddr.clear();
    endAddr.clear();

    markerNames.clear();
    markerAddrs.clear();
    markerPoints.clear();

    strokeColor = "#FF0000";
    strokeWeight = 3;
    strokeOpacity = 0.5;
    overlayTypes.clear();
    overlayPoints.clear();
}

QString MapBaiDu::newMap()
{
    QStringList list;

    //添加头部
    addHead(list);
    //添加body
    addBody(list);
    //添加地图属性
    addProperty(list);
    //添加JS函数
    addFunction(list);
    //添加多个设备
    addDevice(list);
    //添加覆盖物工具栏
    addOverlayTool(list);
    //添加多个覆盖物
    addOverlay(list);
    //添加路线查询
    addRoute(list);
    //添加尾部
    addEnd(list);

    //保存文件
    QString content = list.join("\r\n");
    if (saveFile && !fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream out(&file);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
            out.setCodec("utf-8");
#endif
            out << content;
        }
    }

    return content;
}

void MapBaiDu::setHeight(int height)
{
    this->height = height;
}

void MapBaiDu::setMapLocal(bool mapLocal)
{
    this->mapLocal = mapLocal;
}

void MapBaiDu::setSaveFile(bool saveFile)
{
    this->saveFile = saveFile;
}

bool MapBaiDu::getSaveFile()
{
    return this->saveFile;
}

void MapBaiDu::setTitle(const QString &title)
{
    this->title = title;
}

void MapBaiDu::setFileName(const QString &fileName)
{
    this->fileName = fileName;
}

void MapBaiDu::setCityJsName(const QString &cityJsName)
{
    this->cityJsName = cityJsName;
}

void MapBaiDu::setMapFlag(const QString &mapFlag)
{
    this->mapFlag = mapFlag;
}

void MapBaiDu::setMapVersionKey(const QString &mapVersionKey)
{
    this->mapVersionKey = mapVersionKey;
}

void MapBaiDu::setMapStyleName(const QString &mapStyleName)
{
    this->mapStyleName = mapStyleName;
}

void MapBaiDu::setMapCenterPoint(const QString &mapCenterPoint)
{
    this->mapCenterPoint = mapCenterPoint;
}

void MapBaiDu::setMapCenterCity(const QString &mapCenterCity)
{
    this->mapCenterCity = mapCenterCity;
}

void MapBaiDu::setMapZoom(quint8 mapZoom)
{
    this->mapZoom = mapZoom;
}

quint8 MapBaiDu::getMapZoom()
{
    return this->mapZoom;
}

void MapBaiDu::setMapMinZoom(quint8 mapMinZoom)
{
    this->mapMinZoom = mapMinZoom;
}

void MapBaiDu::setMapMaxZoom(quint8 mapMaxZoom)
{
    this->mapMaxZoom = mapMaxZoom;
}

void MapBaiDu::setEnableMapClick(bool enableMapClick)
{
    this->enableMapClick = enableMapClick;
}

void MapBaiDu::setEnableDragging(bool enableDragging)
{
    this->enableDragging = enableDragging;
}

void MapBaiDu::setEnableScrollWheelZoom(bool enableScrollWheelZoom)
{
    this->enableScrollWheelZoom = enableScrollWheelZoom;
}

void MapBaiDu::setEnableDoubleClickZoom(bool enableDoubleClickZoom)
{
    this->enableDoubleClickZoom = enableDoubleClickZoom;
}

void MapBaiDu::setEnableKeyboard(bool enableKeyboard)
{
    this->enableKeyboard = enableKeyboard;
}

void MapBaiDu::setEnableAnimation(bool enableAnimation)
{
    this->enableAnimation = enableAnimation;
}

void MapBaiDu::setEnableClickPoint(bool enableClickPoint)
{
    this->enableClickPoint = enableClickPoint;
}

void MapBaiDu::setShowNavigationControl(bool showNavigationControl)
{
    this->showNavigationControl = showNavigationControl;
}

void MapBaiDu::setShowOverviewMapControl(bool showOverviewMapControl)
{
    this->showOverviewMapControl = showOverviewMapControl;
}

void MapBaiDu::setShowScaleControl(bool showScaleControl)
{
    this->showScaleControl = showScaleControl;
}

void MapBaiDu::setShowMapTypeControl(bool showMapTypeControl)
{
    this->showMapTypeControl = showMapTypeControl;
}

void MapBaiDu::setShowPanoramaCoverageLayer(bool showPanoramaCoverageLayer)
{
    this->showPanoramaCoverageLayer = showPanoramaCoverageLayer;
}

void MapBaiDu::setShowTrafficControl(bool showTrafficControl)
{
    this->showTrafficControl = showTrafficControl;
}

void MapBaiDu::setShowOverlayTool(bool showOverlayTool)
{
    this->showOverlayTool = showOverlayTool;
}

void MapBaiDu::setStrokeColor(const QString &strokeColor)
{
    this->strokeColor = strokeColor;
}

void MapBaiDu::setStrokeWeight(int strokeWeight)
{
    this->strokeWeight = strokeWeight;
}

void MapBaiDu::setStrokeOpacity(float strokeOpacity)
{
    this->strokeOpacity = strokeOpacity;
}

void MapBaiDu::setMarkerInfo(const QStringList &markerNames, const QStringList &markerAddrs, const QStringList &markerPoints)
{
    this->markerNames = markerNames;
    this->markerAddrs = markerAddrs;
    this->markerPoints = markerPoints;
}

void MapBaiDu::setRotueInfo(quint8 routeType, quint8 policyType, const QString &startAddr, const QString &endAddr)
{
    this->routeType = routeType;
    this->policyType = policyType;
    this->startAddr = startAddr;
    this->endAddr = endAddr;
}

void MapBaiDu::setOverlayInfo(QList<OverlayType> &overlayTypes, const QList<QStringList> &overlayPoints)
{
    this->overlayTypes = overlayTypes;
    this->overlayPoints = overlayPoints;
}
