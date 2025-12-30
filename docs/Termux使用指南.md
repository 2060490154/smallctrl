# Termux 完整使用指南

## 目录
1. [Termux 简介](#termux-简介)
2. [安装与初始配置](#安装与初始配置)
3. [基础命令](#基础命令)
4. [包管理器使用](#包管理器使用)
5. [文件系统访问](#文件系统访问)
6. [开发环境搭建](#开发环境搭建)
7. [高级功能](#高级功能)
8. [常见问题解决](#常见问题解决)

---

## Termux 简介

Termux 是一个 Android 终端模拟器和 Linux 环境应用程序，无需 root 权限即可运行。它提供了一个完整的 Linux 命令行环境。

### 主要特点
- 无需 root 权限
- 提供完整的包管理系统（APT）
- 支持多种编程语言
- 可以访问设备存储
- 支持 SSH、FTP 等网络协议

---

## 安装与初始配置

### 安装 Termux
1. 从 F-Droid 下载安装（推荐）
   - 访问: https://f-droid.org/packages/com.termux/
2. 或从 GitHub Releases 下载
   - 访问: https://github.com/termux/termux-app/releases

### 初始配置

```bash
# 更新软件包列表
pkg update

# 升级所有已安装的软件包
pkg upgrade

# 安装必要的工具
pkg install vim curl wget git
```

### 更换镜像源（加速下载）

```bash
# 编辑源列表
vim $PREFIX/etc/apt/sources.list

# 替换为清华源
deb https://mirrors.tuna.tsinghua.edu.cn/termux/apt/termux-main stable main

# 更新软件包列表
pkg update
```

---

## 基础命令

### 导航命令
```bash
# 显示当前目录
pwd

# 列出文件和目录
ls
ls -la    # 详细列表，包括隐藏文件

# 切换目录
cd /path/to/directory
cd ~      # 切换到家目录
cd ..     # 返回上一级目录
cd -      # 返回上次所在目录
```

### 文件操作
```bash
# 创建文件
touch filename.txt

# 创建目录
mkdir directory_name
mkdir -p dir1/dir2/dir3  # 创建多级目录

# 复制文件
cp source.txt destination.txt
cp -r source_dir destination_dir  # 复制目录

# 移动/重命名文件
mv old_name.txt new_name.txt

# 删除文件
rm filename.txt
rm -rf directory_name  # 删除目录及其内容（谨慎使用）
```

### 查看文件内容
```bash
# 查看整个文件
cat filename.txt

# 分页查看
less filename.txt
more filename.txt

# 查看文件开头
head filename.txt
head -n 20 filename.txt  # 查看前20行

# 查看文件结尾
tail filename.txt
tail -f logfile.txt  # 实时查看文件更新
```

---

## 包管理器使用

### pkg 命令（推荐）
```bash
# 搜索软件包
pkg search package_name

# 安装软件包
pkg install package_name

# 卸载软件包
pkg uninstall package_name

# 更新软件包列表
pkg update

# 升级所有软件包
pkg upgrade

# 列出已安装的软件包
pkg list-installed

# 显示软件包信息
pkg show package_name
```

### apt 命令（与 pkg 类似）
```bash
apt update
apt upgrade
apt install package_name
apt remove package_name
apt search package_name
```

### 常用软件包推荐
```bash
# 开发工具
pkg install python nodejs ruby golang rust clang

# 文本编辑器
pkg install vim nano emacs

# 网络工具
pkg install curl wget openssh rsync

# 版本控制
pkg install git

# 数据库
pkg install postgresql mariadb redis

# 其他工具
pkg install htop tmux screen zip unzip
```

---

## 文件系统访问

### Termux 文件系统结构
```bash
# Termux 的根目录
$PREFIX              # 通常是 /data/data/com.termux/files/usr
$HOME                # 用户主目录 /data/data/com.termux/files/home
```

### 访问设备存储
```bash
# 授予存储权限
termux-setup-storage

# 访问共享存储目录
cd ~/storage/shared          # 内部存储
cd ~/storage/downloads       # 下载文件夹
cd ~/storage/dcim           # 相机照片
cd ~/storage/pictures       # 图片文件夹
cd ~/storage/music          # 音乐文件夹
```

### 文件传输
```bash
# 使用 termux-api 访问设备功能
pkg install termux-api

# 分享文件
termux-share filename.txt

# 打开文件
termux-open filename.pdf
```

---

## 开发环境搭建

### Python 环境
```bash
# 安装 Python
pkg install python

# 安装 pip
pkg install python-pip

# 安装虚拟环境
pip install virtualenv

# 创建虚拟环境
virtualenv myenv
source myenv/bin/activate

# 安装常用库
pip install numpy pandas requests flask django
```

### Node.js 环境
```bash
# 安装 Node.js 和 npm
pkg install nodejs

# 验证安装
node --version
npm --version

# 安装全局包
npm install -g yarn typescript nodemon

# 创建项目
mkdir myproject
cd myproject
npm init -y
```

### Web 服务器
```bash
# 安装 Apache
pkg install apache2

# 启动 Apache
apachectl start

# 安装 Nginx
pkg install nginx

# 启动 Nginx
nginx
```

### Git 和 GitHub
```bash
# 安装 Git
pkg install git

# 配置 Git
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# 生成 SSH 密钥
pkg install openssh
ssh-keygen -t rsa -b 4096 -C "your.email@example.com"

# 查看公钥
cat ~/.ssh/id_rsa.pub

# 克隆仓库
git clone https://github.com/username/repo.git
```

---

## 高级功能

### SSH 远程连接

#### SSH 客户端
```bash
# 安装 OpenSSH
pkg install openssh

# 连接远程服务器
ssh username@remote_host

# 使用密钥连接
ssh -i ~/.ssh/private_key username@remote_host

# 复制文件到远程
scp local_file username@remote_host:/path/to/destination

# 从远程复制文件
scp username@remote_host:/path/to/file local_destination
```

#### SSH 服务器（在 Termux 中运行）
```bash
# 安装 OpenSSH
pkg install openssh

# 设置密码
passwd

# 启动 SSH 服务
sshd

# 查看用户名和端口
whoami
echo $PREFIX/var/run/sshd.pid

# 从其他设备连接
# ssh -p 8022 username@termux_device_ip
```

### 使用 Tmux/Screen 多窗口
```bash
# 安装 Tmux
pkg install tmux

# 启动 Tmux
tmux

# Tmux 基本操作（Ctrl+b 是前缀键）
# Ctrl+b c     创建新窗口
# Ctrl+b n     切换到下一个窗口
# Ctrl+b p     切换到上一个窗口
# Ctrl+b d     分离会话
# Ctrl+b %     垂直分割窗格
# Ctrl+b "     水平分割窗格

# 重新连接会话
tmux attach
```

### Cron 定时任务
```bash
# 安装 Cron
pkg install cronie

# 启动 Cron 守护进程
crond

# 编辑定时任务
crontab -e

# 示例：每天凌晨2点执行备份脚本
# 0 2 * * * /path/to/backup.sh

# 列出定时任务
crontab -l
```

### Termux API
```bash
# 安装 Termux API
pkg install termux-api

# 同时需要安装 Termux:API 应用（从 F-Droid）

# 常用 API 命令
termux-battery-status    # 电池状态
termux-camera-photo      # 拍照
termux-clipboard-get     # 获取剪贴板
termux-clipboard-set     # 设置剪贴板
termux-location         # 获取位置信息
termux-notification     # 发送通知
termux-toast           # 显示提示信息
termux-vibrate         # 震动
termux-wifi-connectioninfo  # WiFi 信息
```

---

## 常见问题解决

### 问题1：无法安装软件包
```bash
# 解决方案：更新软件包列表
pkg update
pkg upgrade

# 清理包缓存
pkg clean
apt clean
```

### 问题2：空间不足
```bash
# 查看磁盘使用情况
df -h

# 清理包缓存
pkg clean

# 卸载不需要的软件包
pkg uninstall package_name

# 清理孤立的依赖
pkg autoremove
```

### 问题3：权限被拒绝
```bash
# 对于存储访问
termux-setup-storage

# 对于文件权限
chmod +x filename.sh
```

### 问题4：中文显示乱码
```bash
# 设置语言环境
export LANG=zh_CN.UTF-8

# 添加到启动文件
echo "export LANG=zh_CN.UTF-8" >> ~/.bashrc
source ~/.bashrc
```

### 问题5：无法连接到网络
```bash
# 检查网络连接
ping -c 4 google.com

# 检查 DNS
nslookup google.com

# 更改 DNS（编辑 resolv.conf）
echo "nameserver 8.8.8.8" > $PREFIX/etc/resolv.conf
```

---

## 实用技巧

### 自定义启动脚本
```bash
# 编辑 .bashrc 文件
vim ~/.bashrc

# 添加别名
alias ll='ls -la'
alias update='pkg update && pkg upgrade'

# 添加环境变量
export PATH=$PATH:$HOME/bin

# 重新加载配置
source ~/.bashrc
```

### 美化终端
```bash
# 安装 Oh My Zsh
pkg install zsh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"

# 修改主题（编辑 ~/.zshrc）
# ZSH_THEME="robbyrussell"
```

### 备份和恢复
```bash
# 备份 Termux
tar -zcvf termux-backup.tar.gz -C /data/data/com.termux/files ./home ./usr

# 恢复 Termux
cd /data/data/com.termux/files
tar -zxvf /sdcard/termux-backup.tar.gz --recursive-unlink --preserve-permissions
```

---

## 学习资源

### 官方文档
- Termux Wiki: https://wiki.termux.com/
- GitHub: https://github.com/termux/termux-app

### 社区
- Termux Reddit: https://reddit.com/r/termux
- Termux Gitter: https://gitter.im/termux/termux

### 相关教程
- Termux 入门指南
- Linux 命令行基础
- Shell 脚本编程

---

## 总结

Termux 是一个功能强大的 Android 终端工具，通过本指南你应该能够：
1. ✅ 安装和配置 Termux
2. ✅ 使用基本的 Linux 命令
3. ✅ 管理软件包和依赖
4. ✅ 搭建开发环境
5. ✅ 使用高级功能如 SSH、Tmux 等
6. ✅ 解决常见问题

继续实践和探索，你将能够充分利用 Termux 的强大功能！
