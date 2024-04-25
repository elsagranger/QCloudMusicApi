# QCloudMusicApi
Qt版 网易云音乐 API

## 简介
本项目翻译自Node.js项目[Binaryify/NeteaseCloudMusicApi](https://github.com/Binaryify/NeteaseCloudMusicApi)

更新尽量与原项目同步，实现了部分接口

使用方式请参考[原项目文档](https://binaryify.github.io/NeteaseCloudMusicApi)，参数与返回结果与原项目完全一致；支持设置Http服务器（需要Qt6.4及以上），运行ApiServer即可启动本地Http服务器

## 需求和依赖
本项目需要以下库：

- Qt >= 5.12

## 使用说明
### 引用动态库文件
在CMakeLists.txt文件中加上
```CMake
# 指定第三方库的路径
set(LIBRARY_DIR "${PROJECT_SOURCE_DIR}/QCloudMusicApi")

# 添加头文件路径
include_directories(${LIBRARY_DIR}/include)

# 添加库文件路径
link_directories(${LIBRARY_DIR}/bin)
```

#### 使用ApiHelper类
该类实现了API的cookie管理

代码中包含头文件apihelper.h
```C++
#include "apihelper.h"
```
通过对应方法字符串调用
```C++
ApiHelper helper;
qDebug() << helper.invoke("lyric", { { "id", 1408586353 } });
```
直接调用API的方法
```C++
ApiHelper helper;
qDebug() << helper.invoke(&NeteaseCloudMusicApi::lyric, { { "id", 1408586353 } });
```

调用ApiHelper::set_cookie设置cookie，支持QVariantMap和QString
#### 使用NeteaseCloudMusicApi类
若无需保存cookie，可直接使用该类

代码中包含头文件module.h
```C++
#include "module.h"
```

可直接调用类函数
```C++
NeteaseCloudMusicApi api;
qDebug() << api.lyric({ { "id", 1408586353 } });
```

也可以通过Qt的反射调用
```C++
NeteaseCloudMusicApi api;
QVariantMap ret;
QMetaObject::invokeMethod(&api, "lyric"
                              , Qt::DirectConnection
                              , Q_RETURN_ARG(QVariantMap, ret)
                              , Q_ARG(QVariantMap, QVariantMap({ {"id", 1408586353} }));
qDebug() << ret;
```

### 在项目中引用CMake项目
在CMakeLists.txt文件中加上
```CMake
add_subdirectory(QCloudMusicApi)

add_executable(YourProjectName
    ./QCloudMusicApi/QCloudMusicApi/module.h
  main.cpp
)

target_compile_definitions(YourProjectName PRIVATE QCLOUDMUSICAPI_LIBRARY)
# 添加需要链接的库
target_link_libraries(YourProjectName  QCloudMusicApi)
```

C++代码中加上
```C++
#include "QCloudMusicApi/QCloudMusicApi/module.h"
```

### 手动编译
手动安装好Qt库后
```Shell
git clone https://github.com/s12mmm3/QCloudMusicApi.git
cmake -B build
cmake --build build -j
```
## License

[The MIT License (MIT)](https://github.com/s12mmm3/QCloudMusicApi/blob/master/LICENSE)
