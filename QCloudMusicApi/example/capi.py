import ctypes
import json
import platform
# import os

# 设置环境变量，不输出Debug级别日志
# os.putenv('QT_LOGGING_RULES', '*.debug=false')

def getSuffix():
    return {
        "Windows": "dll",
        "Linux": "so",
        "Darwin": "dylib",
    }.get(platform.system())

lib = ctypes.CDLL(f"./QCloudMusicApi.{getSuffix()}")

lib.invoke.argtypes = [ ctypes.c_char_p, ctypes.c_char_p ]
lib.invoke.restype = ctypes.c_char_p

lib.invokeUrl.argtypes = [ ctypes.c_char_p ]
lib.invokeUrl.restype = ctypes.c_char_p

lib.freeApi.restype = ctypes.c_void_p
lib.memberName.restype = ctypes.c_char_p
lib.memberCount.restype = ctypes.c_int

lib.set_cookie.argtypes = [ ctypes.c_char_p ]
lib.set_cookie.restype = ctypes.c_void_p

lib.cookie.argtypes = [ ]
lib.cookie.restype = ctypes.c_char_p

lib.set_proxy.argtypes = [ ctypes.c_char_p ]
lib.set_proxy.restype = ctypes.c_void_p

lib.proxy.argtypes = [ ]
lib.proxy.restype = ctypes.c_char_p

lib.setFilterRules.argtypes = [ ctypes.c_char_p ]
lib.setFilterRules.restype = ctypes.c_void_p

lib.loadPlugin.argtypes = [ ctypes.c_char_p ]
lib.loadPlugin.restype = ctypes.c_bool

lib.unloadPlugin.argtypes = [ ctypes.c_char_p ]
lib.unloadPlugin.restype = ctypes.c_bool

# 获取API列表
def memberList():
    list = []
    for i in range(0, lib.memberCount()):
        list.append(lib.memberName(i).decode())
    return list

# 反射调用API的成员函数
def invoke(name, value):
    result = lib.invoke(ctypes.create_string_buffer(name.encode()),
                         ctypes.create_string_buffer(value.encode()))
    return result.decode()

# 反射调用API的成员函数
def invokeUrl(url):
    result = lib.invokeUrl(ctypes.create_string_buffer(url.encode()))
    return result.decode()

# 设置全局cookie
def set_cookie(cookie):
    lib.set_cookie(ctypes.create_string_buffer(cookie.encode()))

# 获取cookie
def cookie():
    result = lib.cookie().decode()
    return result

# 设置全局代理
def set_proxy(proxy):
    lib.set_proxy(ctypes.create_string_buffer(proxy.encode()))

# 获取代理
def proxy():
    result = lib.proxy().decode()
    return result

# 设置log规则
def setFilterRules(rules):
    lib.setFilterRules(ctypes.create_string_buffer(rules.encode()))

# 加载插件
def loadPlugin(fileName):
    return lib.loadPlugin(ctypes.create_string_buffer(fileName.encode()))

# 卸载插件
def unloadPlugin(fileName):
    return lib.unloadPlugin(ctypes.create_string_buffer(fileName.encode()))

if __name__ == '__main__':
    setFilterRules("QCloudMusicApi.debug=false")

    result = invoke("lyric_new", json.dumps({
        "id": "2058263032"
    }))
    print("result", json.dumps(json.loads(result), indent = 4, ensure_ascii = False))
    
    lib.freeApi()
