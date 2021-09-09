# Speed-Box 简介
Speed Box 是一款非常方便快捷的网速显示软件，它拥有许多好用的功能，目前支持
- 实时网速——悬浮窗显示网速和内存占用率
- 划词翻译——手动输入中英文翻译、快捷键划词翻译
- 切换壁纸——后台、手动切换[Wallhaven](https://wallhaven.cc/)、[Bing](https://cn.bing.com/)、[Unsplash](https://unsplash.com/)、电脑本地以及所有其它网站的图片。
- 任务栏美化——改变颜色、图标居中、靠右、透明效果、磨砂效果、玻璃效果，时间格式精确到秒。
- 快捷功能——悬浮窗右键快速关机重启、系统右键增加“复制路径”
- 防止息屏——防止电脑休眠，比如静态编译Qt要两个多小时，中途万一电脑节电睡眠就不妙了。
# 功能介绍
在Speed Box的网速悬浮窗处单击右键，即可看到有许多快捷功能，这些功能看文字就很容易知道是什么意思了。再点击其中的“软件设置”可以打开设置界面，可以看到这里有*壁纸设置*、*桌面美化*、*路径设置*、*其它设置*等。
大多数设置都有Tool Tip提示，你只需要把鼠标放在按钮上面悬停一下就可以看到它的功能解释。这里需要详细说明的功能只有三个个——翻译功能、路径设置、壁纸设置中的高级命令。
## [划词翻译]
* 划词翻译功能再实现的过程中调用了百度翻译的api，因此需要app id和密码。如果你在启用了翻译功能，就需要输入它们。我这里暂时提供一个APP ID：**20210503000812254**，密钥：**Q_2PPxmCr66r6B2hi0ts**。注册一个ID是免费的，建议大家自己去[注册](http://api.fanyi.baidu.com/)。
* 在右键菜单**启用**划词翻译后，你可以鼠标**左键双击**悬浮窗，“极简翻译”的窗口就会自己弹出来了，你还可以按下**“Shift+Z”**键，直接呼唤出划词翻译，你也可以在*选中*一段文本后，按下**“Ctrl+C”**键将其复制，再按下**“Shift+Z”**键将其直接翻译。如果启用了fix，“极简翻译”窗口就会不自动消失，如果取消选中"fix"，“极简翻译”就会在10秒钟之后自动消失。此外，你还可以用快捷键**"Shift+A"**关闭“极简翻译”窗口。当输入焦点在“极简翻译”内的时候，你可以**回车直接翻译，按下ALt切换中英文结果，按下delete键清空输入，按Esc关闭窗口**。
## [路径设置]
* 打开**设置窗口-路径设置**后，这里有“标准名称”和“新名称”之分，标准名称就是软件默认使用存放壁纸文件的文件夹的名称，新名称就是你想把它改成的名称。
* 壁纸文件夹默认在“用户/图片”里面的“Wallpapers”目录下，你可以点击“壁纸下载目录”后面的“打开”按钮进行查看。当你想要更换壁纸下载路径时，直接输入路径回车即可。
* 还有一个就是右键菜单的“打开目录功能”，你可以在这里进行自定义每次点击**“右键菜单-打开目录“**时要打开的目录。
## [高级命令]
这里举个例子，比如我会Python，并且我找到了一个好的壁纸网站（比如[极简壁纸](https://bz.zzzmh.cn/index)），我能将它上面的壁纸下载来存储在电脑里面，我就可以把这个爬虫添加到Speed Box里面。这里假设我们在D:\hello.py里面写了爬虫代码并且成功下载了一张图片保存在D:\hello.jpg，我们在D:\hello.py里面注释掉所有的print输出，只保留一句
```python
print("D:\\hello.jpg")
```
这时候我们就可以把Speed Box里面的高级命令设置为
`python D:\hello.py`
然后Speed Box就可以自动调用这个脚本了。其它语言的脚本或者程序也一样，只要输出图片路径，Speed Box就能自动识别，并且能定时调用这个脚本，也可以点击在右键菜单里面点击“换一张图”调用。
    
    
说了这么多，大家也不一定用得到，下面放几张软件截图
# 运行效果
![img](https://gitee.com/yjmthu/Speed-Box/raw/main/img/img_05.png)
    
    
![img](https://gitee.com/yjmthu/Speed-Box/raw/main/img/img_04.png)

# 编译要求
- Qt版本：Qt 6.0.4 或者 Qt 6.1.2
- 编译器：MSVC-2019 64bit
- 使用多字节字符集（所有Windows Api函数都是A版而不是W版）
- 其余细节可自行修改pro文件

# 下载方式

* * *
- 版本：21.6.27（旧）
- 发布信息
  * Qt编写
  * 适用于64位的win10系统。
- 下载地址
  * [蓝奏云](https://wws.lanzoui.com/i4A13qqvcmh) 密码:52ca
  * [GitHub](https://github.com/yjmthu/Speed-Box/releases/download/21.6.27/Speed-Box_win10_x64_21.6.27.zip)

* * *
- 版本：21.8.1.0（第一次更新）
- 更新内容
  * MFC编写
  * 同时支持64位和32位系统
  * 单个文件，2-3MB体积，初始内存只占1-2MB
  * 优化了文件结构
  * 改动了界面样式和颜色
  * 变成鼠标左键双击打开翻译
  * 中键单击重启软件
  * 优化了壁纸下载速度和效率
  * 优化了翻译功能
  * 改动右键时不再需要管理员权限
  * 高级功能要输入python的绝对路径, 并且需要在py文件前加-, 例如 `D:\Ppython3\python.exe -u D:\hello.py`。
- 下载地址
  * 64位系统 [蓝奏云](https://wws.lanzoui.com/iocoqs3ejmb) 密码 33ra
  * 32位系统 [蓝奏云](https://wws.lanzoui.com/ict95s3ejkj) 密码 g99a

* * *
- 版本：21.8.6
- 更新内容
  * 使用最新的Qt6编写
  * 优化了窗口外观
  * 增加了语言自动识别功能
  * 改变了一下文件夹名称
  * 优化了底层算法
  * 优化了使用体验
  * （另外不建议使用上面MFC版本的）
- 下载地址 （64位Windows 10）
  * [GitHub](https://github.com/yjmthu/Speed-Box/releases/download/21.8.6/Speed-Box_win10_x64_21.8.6.zip)

* * *
- 版本：21.8.10 （最新稳定版本）
- 更新内容
  * 可以自动检查更新了！
  * 修复翻译出错的Bug。
  * 由于必应首页的源码进行了重构，所以以前的版本不能使用必应壁纸了。因此这个版本更新了必应壁纸。
  * 改变了界面样式。
  * 优化了一些代码，提高了效率节约内存。
- 下载地址（64位Windows 10）
  * [GitHub](https://github.com/yjmthu/Speed-Box/releases/download/21.8.10/Speed-Box_win10_x64_21.8.10.zip)
  * [Gitee](https://gitee.com/yjmthu/Speed-Box/attach_files/797630/download/Speed-Box_win10_x64_21.8.10.zip)
  * [蓝奏云](https://wws.lanzoui.com/ixjz1sg45ud) 密码：ejv5

* * *
- 版本：21.9.10 (最新稳定版本)
- 更新内容
  * 修复右边图标太多时居中不正常的问题
  * 修复任务栏透明度显示不正常的问题
  * 尝试增加发布者信息
  * 增加一些操作完成提示
  * 解决了下载更新卡住的问题
  * 将开机自启移动到了软件设置-其他设置下
  * 移除了快捷重启功能。
  * 增加了壁纸历史记录功能，可以通过右键菜单来切换上一张图。
  * 增加了壁纸黑名单功能，可以将不喜欢的wallhaven壁纸删除并加入黑名单，将其他类型的壁纸直接删除。
  * 增加了win11检测功能，win11下部分功能无法使用
  * 更改配置文件名称，更改配置文件键盘值对名称
  * 增加主题颜色
  * 完善tooltip提示，前面带*号的表示点击后会保存
  * 移动控件的位置，改变了部分颜色
  * 完全解除更新按钮的禁用
  * 解决了更新中极小概率发生的出错问题
  * 改善了上下壁纸切换逻辑，当列表中的壁纸不存在自动时，将其从列表中移除并跳过。
  * 必应壁纸更加精彩，弃用了从必应首页爬取壁纸的方法，改用必应官方的api，可以设置循环切换最近八天壁纸，可以使用壁纸信息作为壁纸名称，可以自定义每次启动是否后台保存必应壁纸。
  * 路径设置-壁纸下载目录增加了选择文件夹按钮。
  * 增加了刷新功能，可以刷新用了很久的图片集合。
  * 将首次更换壁纸和定时更换壁纸区别开来，可选择软件启动时更换一次壁纸
- 下载地址(64位)
  * Windows 10 [GitHub](https://github.com/yjmthu/Speed-Box/releases/download/21.9.10/Speed-Box_win10x64_21.9.10.zip)
  * Windows 10 [Gitee](https://gitee.com/yjmthu/Speed-Box/attach_files/797630/download/Speed-Box_win10_x64_21.8.10.zip)
  * Windows 10 [蓝奏云](https://wws.lanzoui.com/i2CHhttejef) 密码：3rvr
  * Windows 11 [GitHub](https://github.com/yjmthu/Speed-Box/releases/download/21.9.10/Speed-Box_win11x64_21.9.10.zip)
  * Windows 11 [Gitee](https://gitee.com/yjmthu/Speed-Box/attach_files/797630/download/Speed-Box_win10_x64_21.8.10.zip)
  * Windows 11 [蓝奏云](https://wws.lanzoui.com/isKtpttejpg) 密码：402u
