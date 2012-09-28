var NAVTREE =
[
  [ "CubeAnimator", "index.html", [
    [ "Class List", "annotated.html", [
      [ "CubeFaceData", "class_cube_face_data.html", null ],
      [ "CubeFaceLabel", "class_cube_face_label.html", null ],
      [ "CubeMapLoader", "class_cube_map_loader.html", null ],
      [ "CubeMapPreview", "class_cube_map_preview.html", null ],
      [ "CurveCursor", "class_curve_cursor.html", null ],
      [ "CurveEditor", "class_curve_editor.html", null ],
      [ "CurveHandleItem", "class_curve_handle_item.html", null ],
      [ "CurveItem", "class_curve_item.html", null ],
      [ "CurveListItemDelegate", "class_curve_list_item_delegate.html", null ],
      [ "CurveListItemWidget", "class_curve_list_item_widget.html", null ],
      [ "CurvesList", "class_curves_list.html", null ],
      [ "CurvesView", "class_curves_view.html", null ],
      [ "CurveVariable", "class_curve_variable.html", null ],
      [ "CurveVariableListModel", "class_curve_variable_list_model.html", null ],
      [ "FramesScene", "class_frames_scene.html", null ],
      [ "IOWidget", "class_i_o_widget.html", null ],
      [ "JSonIO", "class_j_son_i_o.html", null ],
      [ "KeyFrames", "class_key_frames.html", null ],
      [ "MainWidget", "class_main_widget.html", null ],
      [ "MainWindow", "class_main_window.html", null ],
      [ "QJson::Parser", "class_q_json_1_1_parser.html", null ],
      [ "QJson::ParserRunnable", "class_q_json_1_1_parser_runnable.html", null ],
      [ "QJson::QObjectHelper", "class_q_json_1_1_q_object_helper.html", null ],
      [ "RenderThread", "class_render_thread.html", null ],
      [ "Ruler", "class_ruler.html", null ],
      [ "QJson::Serializer", "class_q_json_1_1_serializer.html", null ],
      [ "QJson::SerializerRunnable", "class_q_json_1_1_serializer_runnable.html", null ],
      [ "TimeLine", "class_time_line.html", null ],
      [ "VariableColorPalette", "class_variable_color_palette.html", null ]
    ] ],
    [ "Class Index", "classes.html", null ],
    [ "Class Members", "functions.html", null ],
    [ "File List", "files.html", [
      [ "Common/convertutil.h", "convertutil_8h.html", null ],
      [ "Common/cubemapdef.h", "cubemapdef_8h.html", null ],
      [ "Common/curvehandleitem.h", null, null ],
      [ "Common/curveitem.h", null, null ],
      [ "Common/curvevariable.h", null, null ],
      [ "Common/fileutil.h", "fileutil_8h.html", null ],
      [ "Common/keyframe.h", null, null ],
      [ "Common/ruler.h", null, null ],
      [ "Common/variablecolorpalette.h", null, null ],
      [ "cubemap2fisheye/cubefacedata.h", null, null ],
      [ "CubeMapLoader/cubefacelabel.h", null, null ],
      [ "CubeMapLoader/cubemaploader.h", null, null ],
      [ "CubeMapPreview/cubemappreview.h", null, null ],
      [ "CurveEditor/curvecursor.h", null, null ],
      [ "CurveEditor/curveeditor.h", null, null ],
      [ "CurveEditor/curvelistitemwidget.h", null, null ],
      [ "CurveEditor/curveslist.h", null, null ],
      [ "CurveEditor/curvesview.h", null, null ],
      [ "ExportModule/iowidget.h", null, null ],
      [ "ExportModule/jsonio.h", null, null ],
      [ "ExportModule/renderthread.h", null, null ],
      [ "FisheyeAnimator/mainwidget.h", null, null ],
      [ "FisheyeAnimator/mainwindow.h", null, null ],
      [ "include/qjson/parser.h", null, null ],
      [ "include/qjson/parserrunnable.h", null, null ],
      [ "include/qjson/qjson_export.h", null, null ],
      [ "include/qjson/qobjecthelper.h", null, null ],
      [ "include/qjson/serializer.h", null, null ],
      [ "include/qjson/serializerrunnable.h", null, null ],
      [ "TimeLine/framesscene.h", null, null ],
      [ "TimeLine/timeline.h", null, null ]
    ] ],
    [ "File Members", "globals.html", null ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

