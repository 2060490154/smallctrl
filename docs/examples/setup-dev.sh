#!/bin/bash
# 开发环境快速设置脚本
# 一键安装常用开发工具和配置环境

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# 打印函数
print_header() {
    echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║  $1${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
}

print_info() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[!]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

# 检查并安装包
install_package() {
    PKG=$1
    if pkg list-installed | grep -q "^$PKG/"; then
        print_info "$PKG 已安装"
    else
        print_warn "正在安装 $PKG..."
        pkg install -y $PKG
        if [ $? -eq 0 ]; then
            print_info "$PKG 安装成功"
        else
            print_error "$PKG 安装失败"
            return 1
        fi
    fi
}

# 基础工具
install_basic_tools() {
    print_header "安装基础工具"
    
    TOOLS=("vim" "curl" "wget" "git" "openssh" "zip" "unzip" "tar")
    
    for tool in "${TOOLS[@]}"; do
        install_package $tool
    done
    
    echo ""
}

# Python 环境
install_python() {
    print_header "配置 Python 环境"
    
    install_package "python"
    install_package "python-pip"
    
    if command -v pip &> /dev/null; then
        print_warn "升级 pip..."
        pip install --upgrade pip
        
        print_warn "安装常用 Python 库..."
        pip install requests beautifulsoup4 flask django numpy pandas jupyter
        
        print_info "Python 环境配置完成"
    else
        print_error "pip 未正确安装"
    fi
    
    echo ""
}

# Node.js 环境
install_nodejs() {
    print_header "配置 Node.js 环境"
    
    install_package "nodejs"
    
    if command -v npm &> /dev/null; then
        print_info "Node.js 版本: $(node --version)"
        print_info "npm 版本: $(npm --version)"
        
        print_warn "安装常用全局包..."
        npm install -g yarn typescript nodemon eslint prettier
        
        print_info "Node.js 环境配置完成"
    else
        print_error "npm 未正确安装"
    fi
    
    echo ""
}

# 数据库
install_databases() {
    print_header "安装数据库"
    
    echo "选择要安装的数据库:"
    echo "1) SQLite (轻量级)"
    echo "2) PostgreSQL"
    echo "3) MariaDB (MySQL)"
    echo "4) Redis"
    echo "5) 全部安装"
    echo "0) 跳过"
    
    read -p "请选择 [0-5]: " db_choice
    
    case $db_choice in
        1)
            install_package "sqlite"
            ;;
        2)
            install_package "postgresql"
            print_warn "初始化 PostgreSQL..."
            mkdir -p $PREFIX/var/lib/postgresql
            initdb $PREFIX/var/lib/postgresql
            ;;
        3)
            install_package "mariadb"
            ;;
        4)
            install_package "redis"
            ;;
        5)
            install_package "sqlite"
            install_package "postgresql"
            install_package "mariadb"
            install_package "redis"
            ;;
        0)
            print_info "跳过数据库安装"
            ;;
        *)
            print_warn "无效选择，跳过"
            ;;
    esac
    
    echo ""
}

# 编程语言
install_languages() {
    print_header "安装其他编程语言"
    
    echo "选择要安装的语言:"
    echo "1) Go"
    echo "2) Rust"
    echo "3) Ruby"
    echo "4) PHP"
    echo "5) Java"
    echo "6) 全部安装"
    echo "0) 跳过"
    
    read -p "请选择 [0-6]: " lang_choice
    
    case $lang_choice in
        1)
            install_package "golang"
            ;;
        2)
            install_package "rust"
            ;;
        3)
            install_package "ruby"
            ;;
        4)
            install_package "php"
            ;;
        5)
            install_package "openjdk-17"
            ;;
        6)
            install_package "golang"
            install_package "rust"
            install_package "ruby"
            install_package "php"
            install_package "openjdk-17"
            ;;
        0)
            print_info "跳过编程语言安装"
            ;;
        *)
            print_warn "无效选择，跳过"
            ;;
    esac
    
    echo ""
}

# 开发工具
install_dev_tools() {
    print_header "安装开发工具"
    
    TOOLS=("tmux" "htop" "tree" "jq" "clang" "make" "cmake")
    
    for tool in "${TOOLS[@]}"; do
        install_package $tool
    done
    
    echo ""
}

# 配置 Git
configure_git() {
    print_header "配置 Git"
    
    if command -v git &> /dev/null; then
        read -p "输入你的 Git 用户名: " git_name
        read -p "输入你的 Git 邮箱: " git_email
        
        git config --global user.name "$git_name"
        git config --global user.email "$git_email"
        
        print_info "Git 配置完成"
        
        read -p "是否生成 SSH 密钥？(y/n): " gen_ssh
        if [ "$gen_ssh" = "y" ]; then
            if [ ! -f ~/.ssh/id_rsa ]; then
                ssh-keygen -t rsa -b 4096 -C "$git_email"
                print_info "SSH 密钥已生成"
                print_info "公钥位置: ~/.ssh/id_rsa.pub"
                echo ""
                print_warn "请将以下公钥添加到 GitHub/GitLab:"
                cat ~/.ssh/id_rsa.pub
            else
                print_info "SSH 密钥已存在"
            fi
        fi
    else
        print_error "Git 未安装"
    fi
    
    echo ""
}

# 配置 Shell
configure_shell() {
    print_header "配置 Shell 环境"
    
    # 创建 .bashrc 如果不存在
    if [ ! -f ~/.bashrc ]; then
        touch ~/.bashrc
    fi
    
    # 添加有用的别名
    cat >> ~/.bashrc << 'EOF'

# 自定义别名
alias ll='ls -la'
alias la='ls -A'
alias l='ls -CF'
alias ..='cd ..'
alias ...='cd ../..'
alias update='pkg update && pkg upgrade'
alias install='pkg install'
alias search='pkg search'

# Git 别名
alias gs='git status'
alias ga='git add'
alias gc='git commit'
alias gp='git push'
alias gl='git log'
alias gd='git diff'

# 颜色提示符
PS1='\[\e[32m\]\u@\h\[\e[0m\]:\[\e[34m\]\w\[\e[0m\]\$ '

# 历史记录设置
export HISTSIZE=10000
export HISTFILESIZE=20000
export HISTCONTROL=ignoredups:erasedups

EOF
    
    print_info "Shell 配置已更新"
    print_info "运行 'source ~/.bashrc' 使配置生效"
    
    echo ""
}

# 创建项目目录结构
create_workspace() {
    print_header "创建工作空间"
    
    WORKSPACE="$HOME/workspace"
    
    if [ ! -d "$WORKSPACE" ]; then
        mkdir -p "$WORKSPACE"/{projects,scripts,learn,tmp}
        print_info "创建工作空间: $WORKSPACE"
        print_info "  - $WORKSPACE/projects  (项目目录)"
        print_info "  - $WORKSPACE/scripts   (脚本目录)"
        print_info "  - $WORKSPACE/learn     (学习目录)"
        print_info "  - $WORKSPACE/tmp       (临时目录)"
    else
        print_info "工作空间已存在"
    fi
    
    echo ""
}

# 显示总结
show_summary() {
    print_header "安装总结"
    
    echo "已安装的工具和语言:"
    echo ""
    
    # 检查各种工具
    [ -x "$(command -v python)" ] && echo -e "${GREEN}✓${NC} Python $(python --version 2>&1 | cut -d' ' -f2)"
    [ -x "$(command -v node)" ] && echo -e "${GREEN}✓${NC} Node.js $(node --version)"
    [ -x "$(command -v go)" ] && echo -e "${GREEN}✓${NC} Go $(go version | awk '{print $3}')"
    [ -x "$(command -v rustc)" ] && echo -e "${GREEN}✓${NC} Rust $(rustc --version | awk '{print $2}')"
    [ -x "$(command -v ruby)" ] && echo -e "${GREEN}✓${NC} Ruby $(ruby --version | awk '{print $2}')"
    [ -x "$(command -v php)" ] && echo -e "${GREEN}✓${NC} PHP $(php --version | head -1 | awk '{print $2}')"
    [ -x "$(command -v java)" ] && echo -e "${GREEN}✓${NC} Java $(java -version 2>&1 | head -1 | awk '{print $3}' | tr -d '"')"
    
    echo ""
    echo "下一步建议:"
    echo "1. 运行 'source ~/.bashrc' 加载新配置"
    echo "2. 访问 ~/workspace 开始你的项目"
    echo "3. 查看教程文档学习更多内容"
    
    echo ""
}

# 主菜单
main_menu() {
    clear
    echo -e "${BLUE}"
    echo "╔═══════════════════════════════════════════════════╗"
    echo "║                                                   ║"
    echo "║     Termux 开发环境快速设置工具                  ║"
    echo "║     Development Environment Setup                 ║"
    echo "║                                                   ║"
    echo "╚═══════════════════════════════════════════════════╝"
    echo -e "${NC}"
    
    echo "请选择安装选项:"
    echo ""
    echo "1) 完整安装（推荐）"
    echo "2) 自定义安装"
    echo "3) 只安装基础工具"
    echo "0) 退出"
    echo ""
    
    read -p "请选择 [0-3]: " choice
    
    case $choice in
        1)
            # 完整安装
            print_info "开始完整安装..."
            echo ""
            pkg update && pkg upgrade -y
            install_basic_tools
            install_python
            install_nodejs
            install_dev_tools
            configure_git
            configure_shell
            create_workspace
            show_summary
            ;;
        2)
            # 自定义安装
            pkg update
            install_basic_tools
            
            read -p "安装 Python 环境？(y/n): " install_py
            [ "$install_py" = "y" ] && install_python
            
            read -p "安装 Node.js 环境？(y/n): " install_node
            [ "$install_node" = "y" ] && install_nodejs
            
            install_databases
            install_languages
            install_dev_tools
            configure_git
            configure_shell
            create_workspace
            show_summary
            ;;
        3)
            # 只安装基础工具
            pkg update
            install_basic_tools
            configure_shell
            show_summary
            ;;
        0)
            print_info "退出安装程序"
            exit 0
            ;;
        *)
            print_error "无效选择"
            exit 1
            ;;
    esac
}

# 运行主程序
main_menu
