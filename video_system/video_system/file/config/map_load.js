var offmapcfg = {
  'imgext'      : '.jpg',   //瓦片图的后缀 ------ 根据需要修改，一般是 .png .jpg
  'tiles_dir'   : '',       //普通瓦片图的地址，为空默认在 tiles 目录
  'tiles_hybrid': '',       //卫星瓦片图的地址，为空默认在 tiles_hybrid 目录
  'tiles_self'  : ''        //自定义图层的地址，为空默认在 tiles_self 目录
};

//////////////////下面的保持不动///////////////////////////////////
var scripts = document.getElementsByTagName("script");
var JS__FILE__ = scripts[scripts.length - 1].getAttribute("src");
offmapcfg.home = JS__FILE__.substr(0, JS__FILE__.lastIndexOf("/")+1); //地图API主目录
(function(){
  window.BMap_loadScriptTime = (new Date).getTime();
  //加载地图API主文件
  document.write('<script type="text/javascript" src="'+offmapcfg.home+'map.js?v=2.1"></script>');
  //加载扩展函数
  document.write('<script type="text/javascript" src="'+offmapcfg.home+'map_plus.js?v=2.1"></script>');
  //加载城市坐标
  document.write('<script type="text/javascript" src="'+offmapcfg.home+'map_city.js?v=2.1"></script>');
})();
///////////////////////////////////////////////////////////////////
