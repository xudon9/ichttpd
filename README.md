# **Ichttpd** V0.2 is here!
'New' Feature: 新增 POST 方法支持.

## Introduction
「 **Ichttpd** 」 是一个简单的支持最常见的 _POST_ 和 _GET_ 方法的 httpd 服务器.

## Clone & Compile
1. 执行命令 `git clone https://github.com/whst/ichttpd.git`;
2. 进入克隆得到的目录 `cd ichttpd`, 执行 `make` 编译.

## Test
1. 如果编译无错误, 执行 `./ichttpd`;
2. 在浏览器内打开网页 `http://localhost:8000/` 查看示例页面.

你可以更改 **ichttpd.conf** 这个配置文件来更改服务器端口号和服务器根目录。

注意，目前服务器还没有提供对 GET 和 POST 以外的 method 的支持。  

## Bugs
在 `Debian GNU/Linux 8.2 (jessie)` 上编译通过。理论上可以运行于所有 POSIX 兼容的 UNIX 类操作系统上，比如 Linux, BSD, OS X.
如果发现 Bug, 请告诉我. 邮箱是: `hsu[AT]whu.edu.cn`

有个小彩蛋 ;-)

> @Wang Hsutung, 2015年 09月 13日 星期日
