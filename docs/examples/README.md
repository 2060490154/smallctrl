# Termux 和 Linux 实用脚本示例

本目录包含了一系列实用的 Shell 脚本，帮助你更好地使用 Termux 和 Linux。

## 📜 脚本列表

### 1. backup.sh - 系统备份脚本
自动备份 Termux 系统和重要文件。

**功能:**
- 自动备份家目录和配置文件
- 排除不必要的文件（缓存、临时文件等）
- 自动清理7天前的旧备份
- 显示备份文件列表和大小
- 彩色输出和进度提示

**使用方法:**
```bash
# 执行备份
chmod +x backup.sh
./backup.sh

# 只查看备份列表
./backup.sh list

# 只清理旧备份
./backup.sh cleanup
```

**配置:**
编辑脚本中的以下变量来自定义：
- `BACKUP_DIR`: 备份文件保存位置
- `SOURCE_DIRS`: 要备份的目录列表

---

### 2. monitor.sh - 系统监控脚本
实时监控系统状态和资源使用情况。

**功能:**
- 显示系统信息（主机名、内核版本、运行时间）
- 监控 CPU 使用率
- 监控内存使用情况（带进度条）
- 显示磁盘使用情况
- 显示 Top 5 CPU 和内存占用进程
- 显示网络信息和连接状态
- 支持持续监控模式

**使用方法:**
```bash
# 单次显示
chmod +x monitor.sh
./monitor.sh

# 持续监控模式（每5秒刷新）
./monitor.sh -c
./monitor.sh --continuous

# 显示帮助
./monitor.sh -h
./monitor.sh --help
```

**快捷键:**
- 在持续监控模式下按 `Ctrl+C` 退出

---

### 3. setup-dev.sh - 开发环境快速设置
一键配置完整的开发环境。

**功能:**
- 安装基础工具（vim, git, curl, wget等）
- 配置 Python 环境（包括常用库）
- 配置 Node.js 环境（包括 yarn, typescript 等）
- 安装数据库（SQLite, PostgreSQL, MariaDB, Redis）
- 安装其他编程语言（Go, Rust, Ruby, PHP, Java）
- 安装开发工具（tmux, htop, tree, jq等）
- 配置 Git 用户信息和 SSH 密钥
- 配置 Shell（添加别名和美化提示符）
- 创建项目工作空间

**使用方法:**
```bash
# 运行设置脚本
chmod +x setup-dev.sh
./setup-dev.sh
```

**安装选项:**
1. **完整安装**（推荐）: 安装所有基础工具和常用开发环境
2. **自定义安装**: 选择需要的组件进行安装
3. **只安装基础工具**: 只安装必要的基础工具

**创建的工作空间:**
```
~/workspace/
├── projects/   # 项目目录
├── scripts/    # 脚本目录
├── learn/      # 学习目录
└── tmp/        # 临时目录
```

---

## 🔧 通用使用技巧

### 赋予执行权限
所有脚本在第一次使用前需要赋予执行权限：
```bash
chmod +x script_name.sh
```

### 批量赋予权限
```bash
chmod +x *.sh
```

### 创建别名方便使用
编辑 `~/.bashrc` 添加：
```bash
alias backup='~/path/to/backup.sh'
alias monitor='~/path/to/monitor.sh'
alias setup-dev='~/path/to/setup-dev.sh'
```

然后重新加载配置：
```bash
source ~/.bashrc
```

### 添加到 PATH
将脚本目录添加到 PATH：
```bash
export PATH=$PATH:~/path/to/scripts
echo 'export PATH=$PATH:~/path/to/scripts' >> ~/.bashrc
```

---

## 📋 脚本自定义

### 修改备份脚本

**修改备份目录:**
```bash
# 编辑 backup.sh
BACKUP_DIR="$HOME/storage/shared/backups"  # 修改为你的目录
```

**添加要备份的目录:**
```bash
SOURCE_DIRS=(
    "$HOME"
    "$PREFIX/etc"
    "/your/custom/path"  # 添加你的路径
)
```

**修改清理策略:**
```bash
# 修改保留天数（默认7天）
DAYS_OLD=$(( ($CURRENT_TIME - $FILE_TIME) / 86400 ))
if [ $DAYS_OLD -gt 30 ]; then  # 改为30天
    # ...
fi
```

### 修改监控脚本

**修改刷新间隔:**
```bash
# 在持续监控模式中
sleep 5  # 改为你想要的秒数
```

**修改警告阈值:**
```bash
# CPU 使用率警告
if (( $(echo "$CPU_USAGE > 80" | bc -l) )); then  # 修改80为其他值
    COLOR=$RED
fi

# 内存使用率警告
if [ $PERCENT -gt 80 ]; then  # 修改80为其他值
    COLOR=$RED
fi
```

---

## 🎯 实践建议

### 1. 定期备份
设置定时任务自动备份：
```bash
# 安装 cron
pkg install cronie

# 编辑定时任务
crontab -e

# 添加：每天凌晨2点备份
0 2 * * * /path/to/backup.sh
```

### 2. 系统监控
在启动脚本中添加监控：
```bash
# 添加到 ~/.bashrc
echo ""
echo "系统状态快览:"
~/path/to/monitor.sh
echo ""
```

### 3. 快速开发环境
新设备上快速搭建环境：
```bash
# 下载脚本
wget https://your-repo/setup-dev.sh

# 运行安装
chmod +x setup-dev.sh
./setup-dev.sh
```

---

## 🐛 故障排除

### 脚本无法执行
```bash
# 检查文件权限
ls -l script.sh

# 赋予执行权限
chmod +x script.sh

# 检查 shebang 行
head -1 script.sh  # 应该是 #!/bin/bash
```

### 命令未找到
```bash
# 检查命令是否安装
which command_name

# 安装缺失的包
pkg install package_name
```

### 权限被拒绝
```bash
# 对于存储访问
termux-setup-storage

# 对于文件权限
chmod 755 file
```

---

## 📚 相关文档

- [Termux 使用指南](../Termux使用指南.md)
- [Linux 知识指南](../Linux知识指南.md)
- [主 README](../README.md)

---

## 🤝 贡献

欢迎提交新的实用脚本！请确保：
1. 添加详细的注释
2. 包含使用说明
3. 处理错误情况
4. 提供彩色输出
5. 遵循 Shell 脚本最佳实践

---

## 📝 注意事项

1. **安全性**: 
   - 不要在脚本中硬编码密码
   - 备份文件可能包含敏感信息，注意保护
   - 使用 SSH 密钥时确保私钥安全

2. **兼容性**:
   - 这些脚本主要为 Termux 设计
   - 大部分也可以在标准 Linux 系统上运行
   - 某些功能可能需要调整

3. **性能**:
   - 持续监控会消耗一定资源
   - 备份大文件夹可能需要较长时间
   - 根据需要调整脚本参数

4. **维护**:
   - 定期检查和更新脚本
   - 清理不需要的旧备份
   - 根据实际需求自定义配置

---

**祝你使用愉快！Happy Coding! 🚀**
