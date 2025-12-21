# Linux 完整知识指南

## 目录
1. [Linux 基础概念](#linux-基础概念)
2. [文件系统结构](#文件系统结构)
3. [基础命令详解](#基础命令详解)
4. [文件权限管理](#文件权限管理)
5. [进程管理](#进程管理)
6. [用户和组管理](#用户和组管理)
7. [网络管理](#网络管理)
8. [Shell 脚本编程](#shell-脚本编程)
9. [系统管理](#系统管理)
10. [高级技巧](#高级技巧)

---

## Linux 基础概念

### 什么是 Linux？
Linux 是一个开源的类 Unix 操作系统内核，由 Linus Torvalds 于 1991 年创建。现在通常指基于 Linux 内核的完整操作系统。

### Linux 发行版
- **Debian 系列**: Ubuntu, Debian, Linux Mint
- **Red Hat 系列**: RHEL, CentOS, Fedora
- **Arch 系列**: Arch Linux, Manjaro
- **其他**: openSUSE, Gentoo, Slackware

### Linux 特点
- 开源免费
- 多用户、多任务
- 稳定可靠
- 安全性高
- 良好的可移植性

---

## 文件系统结构

### Linux 目录树结构
```
/                    # 根目录
├── bin             # 基本命令二进制文件
├── boot            # 启动文件
├── dev             # 设备文件
├── etc             # 系统配置文件
├── home            # 用户主目录
├── lib             # 系统库文件
├── media           # 可移动媒体挂载点
├── mnt             # 临时挂载点
├── opt             # 可选应用程序
├── proc            # 进程信息（虚拟文件系统）
├── root            # root 用户主目录
├── run             # 运行时数据
├── sbin            # 系统管理命令
├── srv             # 服务数据
├── sys             # 系统信息（虚拟文件系统）
├── tmp             # 临时文件
├── usr             # 用户程序和数据
└── var             # 可变数据（日志等）
```

### 重要目录说明

#### /etc - 配置文件目录
```bash
/etc/passwd         # 用户信息
/etc/shadow         # 用户密码（加密）
/etc/group          # 组信息
/etc/fstab          # 文件系统挂载配置
/etc/hosts          # 主机名解析
/etc/hostname       # 主机名
/etc/resolv.conf    # DNS 配置
/etc/network/       # 网络配置
/etc/ssh/           # SSH 配置
```

#### /var - 可变数据目录
```bash
/var/log/           # 系统日志
/var/cache/         # 应用缓存
/var/tmp/           # 临时文件（重启保留）
/var/spool/         # 队列数据
/var/www/           # Web 服务器数据
```

---

## 基础命令详解

### 1. 文件和目录操作

#### 查看和导航
```bash
# 显示当前目录
pwd

# 列出文件
ls                  # 简单列表
ls -l               # 详细列表
ls -la              # 包括隐藏文件
ls -lh              # 人类可读的文件大小
ls -ltr             # 按时间排序

# 切换目录
cd /path/to/dir     # 绝对路径
cd ../              # 上一级目录
cd ~                # 家目录
cd -                # 上次所在目录
```

#### 创建和删除
```bash
# 创建目录
mkdir dirname
mkdir -p path/to/dir    # 创建多级目录

# 创建文件
touch filename.txt

# 删除文件
rm filename.txt
rm -f filename.txt      # 强制删除
rm -i filename.txt      # 交互式删除

# 删除目录
rmdir empty_dir         # 只能删除空目录
rm -r dirname           # 递归删除
rm -rf dirname          # 强制递归删除（危险！）
```

#### 复制和移动
```bash
# 复制文件
cp source.txt dest.txt
cp -r source_dir dest_dir   # 复制目录
cp -a source dest           # 保留所有属性
cp -i source dest           # 交互式（覆盖前询问）

# 移动/重命名
mv oldname newname
mv file /path/to/destination/
mv -i source dest           # 交互式移动
```

### 2. 文件内容查看

```bash
# 查看整个文件
cat filename.txt
cat file1 file2 > merged.txt    # 合并文件

# 分页查看
less filename.txt       # 推荐使用
more filename.txt

# less 命令快捷键
# 空格：下一页
# b：上一页
# /pattern：搜索
# n：下一个搜索结果
# q：退出

# 查看文件开头
head filename.txt
head -n 20 filename.txt     # 前20行

# 查看文件结尾
tail filename.txt
tail -n 20 filename.txt     # 后20行
tail -f logfile.txt         # 实时监控（常用于日志）

# 查看文件类型
file filename.txt

# 统计文件信息
wc filename.txt         # 行数、单词数、字节数
wc -l filename.txt      # 只统计行数
```

### 3. 文本搜索和处理

```bash
# grep - 搜索文本
grep "pattern" filename.txt
grep -i "pattern" file          # 忽略大小写
grep -r "pattern" directory/    # 递归搜索
grep -n "pattern" file          # 显示行号
grep -v "pattern" file          # 反向匹配
grep -E "regex" file            # 使用正则表达式

# find - 查找文件
find /path -name "filename"
find /path -name "*.txt"
find /path -type f              # 只查找文件
find /path -type d              # 只查找目录
find /path -size +100M          # 大于100MB的文件
find /path -mtime -7            # 7天内修改的文件
find /path -name "*.log" -delete    # 删除找到的文件

# sed - 流编辑器
sed 's/old/new/' file           # 替换每行第一个匹配
sed 's/old/new/g' file          # 替换所有匹配
sed -i 's/old/new/g' file       # 直接修改文件
sed -n '10,20p' file            # 打印10-20行

# awk - 文本处理工具
awk '{print $1}' file           # 打印第一列
awk -F: '{print $1}' /etc/passwd    # 指定分隔符
awk '$3 > 100' file             # 条件过滤
```

### 4. 文件压缩和归档

```bash
# tar - 归档
tar -cvf archive.tar files/     # 创建归档
tar -xvf archive.tar            # 解压归档
tar -tvf archive.tar            # 查看归档内容

# tar + gzip
tar -czvf archive.tar.gz files/ # 创建压缩归档
tar -xzvf archive.tar.gz        # 解压

# tar + bzip2
tar -cjvf archive.tar.bz2 files/
tar -xjvf archive.tar.bz2

# zip/unzip
zip archive.zip files
zip -r archive.zip directory/
unzip archive.zip
unzip -l archive.zip            # 查看内容

# gzip/gunzip
gzip file.txt                   # 压缩（替换原文件）
gunzip file.txt.gz              # 解压
```

---

## 文件权限管理

### 理解权限

#### 查看权限
```bash
ls -l filename
# 输出示例：-rwxr-xr-- 1 user group 1234 Dec 21 10:00 filename
#          |权限| |所有者| |组| |大小| |时间| |文件名|
```

#### 权限说明
```
-rwxr-xr--
│││││││││└─ 其他用户权限（r--：只读）
││││││└─── 组权限（r-x：读和执行）
│││└────── 所有者权限（rwx：读、写、执行）
│└──────── 文件类型（-：普通文件，d：目录，l：链接）
```

### 权限表示

| 字符 | 数字 | 含义 |
|------|------|------|
| r    | 4    | 读   |
| w    | 2    | 写   |
| x    | 1    | 执行 |

### 修改权限

```bash
# chmod - 修改权限
chmod 755 filename          # rwxr-xr-x
chmod 644 filename          # rw-r--r--
chmod u+x filename          # 所有者添加执行权限
chmod g-w filename          # 组删除写权限
chmod o=r filename          # 其他用户只读
chmod -R 755 directory/     # 递归修改

# 常用权限组合
chmod 777 file              # rwxrwxrwx（所有人所有权限）
chmod 755 file              # rwxr-xr-x（常用于可执行文件）
chmod 644 file              # rw-r--r--（常用于普通文件）
chmod 600 file              # rw-------（私有文件）

# chown - 修改所有者
chown user filename
chown user:group filename
chown -R user:group directory/

# chgrp - 修改组
chgrp group filename
```

---

## 进程管理

### 查看进程

```bash
# ps - 进程状态
ps                      # 当前终端的进程
ps aux                  # 所有进程详细信息
ps -ef                  # 完整格式
ps -u username          # 特定用户的进程

# top - 实时进程监控
top
# 常用快捷键：
# q：退出
# k：终止进程
# M：按内存排序
# P：按CPU排序

# htop - 增强版 top（需要安装）
htop

# pgrep - 查找进程
pgrep firefox
pgrep -u username

# pidof - 根据名称查找PID
pidof nginx
```

### 管理进程

```bash
# 启动进程
command &               # 后台运行
nohup command &         # 后台运行（不受终端关闭影响）

# 终止进程
kill PID                # 发送 TERM 信号
kill -9 PID             # 强制终止
killall process_name    # 按名称终止
pkill pattern           # 按模式终止

# 信号类型
kill -l                 # 列出所有信号
# SIGTERM (15)：正常终止
# SIGKILL (9)：强制终止
# SIGHUP (1)：重新加载配置

# 前后台切换
Ctrl+Z                  # 暂停当前进程
bg                      # 将进程放到后台
fg                      # 将进程放到前台
jobs                    # 查看后台任务

# nice - 设置进程优先级
nice -n 10 command      # 设置优先级
renice -n 5 -p PID      # 修改运行中进程的优先级
```

---

## 用户和组管理

### 用户管理

```bash
# 添加用户
useradd username
useradd -m username         # 创建家目录
useradd -m -s /bin/bash username    # 指定shell

# 设置密码
passwd username

# 修改用户
usermod -l newname oldname  # 修改用户名
usermod -aG groupname username  # 添加到组
usermod -s /bin/zsh username    # 修改shell

# 删除用户
userdel username
userdel -r username         # 删除用户及家目录

# 查看用户信息
id username
whoami
who
w
last                        # 登录历史
```

### 组管理

```bash
# 添加组
groupadd groupname

# 修改组
groupmod -n newname oldname

# 删除组
groupdel groupname

# 查看组
groups username
cat /etc/group
```

### 切换用户

```bash
# su - 切换用户
su username                 # 切换用户
su -                        # 切换到 root
su - username               # 切换并加载环境

# sudo - 以超级用户权限执行
sudo command
sudo -i                     # 切换到 root shell
sudo -u username command    # 以指定用户身份执行

# 配置 sudo
visudo                      # 编辑 /etc/sudoers
# 格式：username ALL=(ALL:ALL) ALL
```

---

## 网络管理

### 网络配置

```bash
# ip - 网络配置工具
ip addr show                # 显示IP地址
ip link show                # 显示网络接口
ip route show               # 显示路由表

# ifconfig（传统工具）
ifconfig                    # 显示网络接口
ifconfig eth0 up            # 启用接口
ifconfig eth0 down          # 禁用接口

# 查看主机名
hostname
hostnamectl                 # systemd 系统

# 设置主机名
hostnamectl set-hostname newhostname
```

### 网络测试

```bash
# ping - 测试连通性
ping google.com
ping -c 4 google.com        # 只发送4个包

# traceroute - 路由跟踪
traceroute google.com

# netstat - 网络状态
netstat -tuln               # 监听端口
netstat -anp                # 所有连接
netstat -r                  # 路由表

# ss - socket 统计（推荐）
ss -tuln                    # 监听端口
ss -s                       # 统计信息

# nslookup / dig - DNS 查询
nslookup google.com
dig google.com

# curl - HTTP 请求
curl http://example.com
curl -O http://example.com/file.zip     # 下载文件
curl -I http://example.com              # 只获取头部

# wget - 下载工具
wget http://example.com/file.zip
wget -c http://example.com/file.zip     # 断点续传
```

### 防火墙

```bash
# iptables（传统）
iptables -L                 # 列出规则
iptables -A INPUT -p tcp --dport 80 -j ACCEPT

# firewall-cmd（RHEL/CentOS）
firewall-cmd --list-all
firewall-cmd --add-port=80/tcp --permanent
firewall-cmd --reload

# ufw（Ubuntu）
ufw status
ufw enable
ufw allow 80/tcp
ufw deny 22/tcp
```

---

## Shell 脚本编程

### 基础脚本结构

```bash
#!/bin/bash
# 这是注释

# 变量
NAME="World"
echo "Hello, $NAME"

# 读取用户输入
read -p "Enter your name: " USERNAME
echo "Hello, $USERNAME"

# 条件语句
if [ $USERNAME == "admin" ]; then
    echo "Welcome admin"
elif [ $USERNAME == "guest" ]; then
    echo "Welcome guest"
else
    echo "Welcome user"
fi

# 循环
for i in {1..5}; do
    echo "Number: $i"
done

while [ $i -le 10 ]; do
    echo $i
    ((i++))
done

# 函数
function greet() {
    echo "Hello, $1"
}
greet "World"

# 命令行参数
echo "Script name: $0"
echo "First argument: $1"
echo "All arguments: $@"
echo "Number of arguments: $#"
```

### 常用条件测试

```bash
# 文件测试
[ -e file ]     # 文件存在
[ -f file ]     # 是普通文件
[ -d dir ]      # 是目录
[ -r file ]     # 可读
[ -w file ]     # 可写
[ -x file ]     # 可执行

# 字符串测试
[ -z string ]   # 字符串为空
[ -n string ]   # 字符串非空
[ str1 == str2 ] # 字符串相等

# 数值测试
[ $a -eq $b ]   # 等于
[ $a -ne $b ]   # 不等于
[ $a -gt $b ]   # 大于
[ $a -lt $b ]   # 小于
[ $a -ge $b ]   # 大于等于
[ $a -le $b ]   # 小于等于
```

### 实用脚本示例

#### 备份脚本
```bash
#!/bin/bash
# backup.sh

SOURCE="/home/user/documents"
DEST="/backup"
DATE=$(date +%Y%m%d)

tar -czf $DEST/backup_$DATE.tar.gz $SOURCE
echo "Backup completed: backup_$DATE.tar.gz"

# 删除7天前的备份
find $DEST -name "backup_*.tar.gz" -mtime +7 -delete
```

#### 系统监控脚本
```bash
#!/bin/bash
# monitor.sh

CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | cut -d'%' -f1)
MEMORY_USAGE=$(free | grep Mem | awk '{print ($3/$2) * 100.0}')
DISK_USAGE=$(df -h / | awk 'NR==2 {print $5}' | cut -d'%' -f1)

echo "CPU Usage: $CPU_USAGE%"
echo "Memory Usage: $MEMORY_USAGE%"
echo "Disk Usage: $DISK_USAGE%"

if [ $(echo "$CPU_USAGE > 80" | bc) -eq 1 ]; then
    echo "WARNING: High CPU usage!"
fi
```

---

## 系统管理

### 系统信息

```bash
# 系统信息
uname -a                # 完整系统信息
uname -r                # 内核版本
cat /etc/os-release     # 发行版信息

# CPU 信息
lscpu
cat /proc/cpuinfo

# 内存信息
free -h
cat /proc/meminfo

# 磁盘信息
df -h                   # 磁盘使用情况
du -sh directory/       # 目录大小
du -h --max-depth=1     # 当前目录下各子目录大小

# 硬件信息
lshw                    # 完整硬件信息
lspci                   # PCI 设备
lsusb                   # USB 设备
lsblk                   # 块设备
```

### 服务管理（systemd）

```bash
# systemctl - 服务管理
systemctl start service_name
systemctl stop service_name
systemctl restart service_name
systemctl status service_name
systemctl enable service_name       # 开机自启
systemctl disable service_name
systemctl list-units --type=service # 列出所有服务

# 查看日志
journalctl
journalctl -u service_name
journalctl -f                       # 实时日志
journalctl --since "1 hour ago"
```

### 包管理

#### Debian/Ubuntu (APT)
```bash
apt update                  # 更新软件包列表
apt upgrade                 # 升级软件包
apt install package_name
apt remove package_name
apt search package_name
apt show package_name
apt autoremove              # 删除不需要的依赖

# dpkg
dpkg -i package.deb         # 安装 deb 包
dpkg -l                     # 列出已安装包
dpkg -r package_name        # 删除包
```

#### RHEL/CentOS (YUM/DNF)
```bash
yum update
yum install package_name
yum remove package_name
yum search package_name

# dnf (新版)
dnf update
dnf install package_name
```

### 定时任务

```bash
# crontab - 定时任务
crontab -e                  # 编辑定时任务
crontab -l                  # 列出定时任务
crontab -r                  # 删除所有定时任务

# crontab 格式
# 分 时 日 月 周 命令
# * * * * * command
# 示例：
# 0 2 * * * /path/to/backup.sh          # 每天凌晨2点
# */5 * * * * /path/to/script.sh        # 每5分钟
# 0 9 * * 1 /path/to/weekly.sh          # 每周一早上9点
```

---

## 高级技巧

### 输入输出重定向

```bash
# 标准输出重定向
command > file              # 覆盖写入
command >> file             # 追加写入

# 标准错误重定向
command 2> error.log
command 2>> error.log

# 同时重定向标准输出和错误
command > output.log 2>&1
command &> output.log

# 输入重定向
command < input.txt

# Here Document
cat << EOF > file.txt
Line 1
Line 2
EOF
```

### 管道和命令组合

```bash
# 管道
command1 | command2
ps aux | grep firefox
cat file.txt | sort | uniq

# tee - 同时输出到文件和标准输出
command | tee output.txt
command | tee -a output.txt     # 追加

# xargs - 参数传递
find . -name "*.txt" | xargs rm
cat urls.txt | xargs -n 1 curl -O
```

### 别名和函数

```bash
# 定义别名
alias ll='ls -la'
alias update='sudo apt update && sudo apt upgrade'

# 查看别名
alias

# 删除别名
unalias ll

# 永久别名（添加到 ~/.bashrc）
echo "alias ll='ls -la'" >> ~/.bashrc
source ~/.bashrc
```

### 环境变量

```bash
# 查看环境变量
env
printenv
echo $PATH
echo $HOME

# 设置环境变量
export VAR_NAME="value"
export PATH=$PATH:/new/path

# 永久设置（添加到 ~/.bashrc 或 ~/.profile）
echo 'export PATH=$PATH:/new/path' >> ~/.bashrc
source ~/.bashrc
```

### 快捷键

```bash
# 命令行快捷键
Ctrl+C          # 中断命令
Ctrl+Z          # 暂停命令
Ctrl+D          # 退出终端/EOF
Ctrl+L          # 清屏
Ctrl+A          # 光标移到行首
Ctrl+E          # 光标移到行尾
Ctrl+U          # 删除光标前的内容
Ctrl+K          # 删除光标后的内容
Ctrl+W          # 删除光标前的单词
Ctrl+R          # 搜索命令历史
!!              # 执行上一条命令
!$              # 上一条命令的最后一个参数
```

### SSH 技巧

```bash
# SSH 密钥认证
ssh-keygen -t rsa -b 4096
ssh-copy-id user@remote_host

# SSH 配置文件 (~/.ssh/config)
Host myserver
    HostName 192.168.1.100
    User username
    Port 22
    IdentityFile ~/.ssh/id_rsa

# 使用配置
ssh myserver

# SSH 隧道
ssh -L 8080:localhost:80 user@remote    # 本地端口转发
ssh -R 8080:localhost:80 user@remote    # 远程端口转发
ssh -D 1080 user@remote                 # 动态端口转发（SOCKS代理）

# SSH 保持连接
# 在 ~/.ssh/config 中添加：
ServerAliveInterval 60
ServerAliveCountMax 3
```

---

## 实战练习

### 练习1：文件管理
```bash
# 创建目录结构
mkdir -p ~/practice/{dir1,dir2,dir3}
cd ~/practice

# 创建文件
for i in {1..5}; do
    touch dir1/file$i.txt
done

# 批量重命名
for file in dir1/*.txt; do
    mv "$file" "${file%.txt}.bak"
done

# 查找并复制
find dir1 -name "*.bak" -exec cp {} dir2/ \;
```

### 练习2：日志分析
```bash
# 查找错误日志
grep -i "error" /var/log/syslog

# 统计访问频率
cat access.log | awk '{print $1}' | sort | uniq -c | sort -rn | head -10

# 分析最常访问的页面
cat access.log | awk '{print $7}' | sort | uniq -c | sort -rn | head -10
```

### 练习3：系统监控
```bash
# 查找占用内存最多的进程
ps aux | sort -nk 4 | tail -5

# 查找占用CPU最多的进程
ps aux | sort -nk 3 | tail -5

# 监控磁盘IO
iostat -x 1 5

# 监控网络流量
iftop           # 需要安装
```

---

## 学习资源

### 在线文档
- Linux Man Pages: https://man7.org/linux/man-pages/
- The Linux Documentation Project: https://tldp.org/
- ArchWiki: https://wiki.archlinux.org/

### 推荐书籍
- 《鸟哥的 Linux 私房菜》
- 《Linux 命令行与 Shell 脚本编程大全》
- 《Unix/Linux 编程实践教程》

### 在线学习平台
- Linux Journey: https://linuxjourney.com/
- OverTheWire: https://overthewire.org/wargames/
- Explain Shell: https://explainshell.com/

---

## 总结

掌握 Linux 需要：
1. ✅ 理解文件系统和目录结构
2. ✅ 熟练使用基本命令
3. ✅ 掌握文件权限和进程管理
4. ✅ 学会 Shell 脚本编程
5. ✅ 了解系统管理和网络配置
6. ✅ 持续实践和探索

记住：**多实践，多查文档（man 命令），不要怕犯错！**

Linux 是一个强大而灵活的系统，随着你的深入学习，你会发现更多有趣和实用的功能。祝你学习愉快！
