#!/bin/bash
# 系统备份脚本
# 用于备份重要文件和配置

# 配置部分
BACKUP_DIR="$HOME/storage/shared/backups"
SOURCE_DIRS=(
    "$HOME"
    "$PREFIX/etc"
)
DATE=$(date +%Y%m%d_%H%M%S)
BACKUP_NAME="termux_backup_$DATE.tar.gz"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 函数：打印信息
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 函数：检查目录是否存在
check_directory() {
    if [ ! -d "$1" ]; then
        print_warn "目录不存在，正在创建: $1"
        mkdir -p "$1"
    fi
}

# 函数：执行备份
perform_backup() {
    print_info "开始备份..."
    print_info "备份目标: $BACKUP_DIR/$BACKUP_NAME"
    
    # 创建备份目录
    check_directory "$BACKUP_DIR"
    
    # 执行备份
    tar -czf "$BACKUP_DIR/$BACKUP_NAME" \
        --exclude='*.tar.gz' \
        --exclude='*.zip' \
        --exclude='cache' \
        --exclude='tmp' \
        "${SOURCE_DIRS[@]}" 2>/dev/null
    
    if [ $? -eq 0 ]; then
        print_info "备份成功完成！"
        print_info "备份文件: $BACKUP_DIR/$BACKUP_NAME"
        
        # 显示备份文件大小
        SIZE=$(du -h "$BACKUP_DIR/$BACKUP_NAME" | cut -f1)
        print_info "备份大小: $SIZE"
    else
        print_error "备份失败！"
        exit 1
    fi
}

# 函数：清理旧备份
cleanup_old_backups() {
    print_info "清理7天前的旧备份..."
    
    DELETED=0
    for file in "$BACKUP_DIR"/termux_backup_*.tar.gz; do
        if [ -f "$file" ]; then
            # 检查文件修改时间
            FILE_TIME=$(stat -c %Y "$file" 2>/dev/null || stat -f %m "$file" 2>/dev/null)
            CURRENT_TIME=$(date +%s)
            DAYS_OLD=$(( ($CURRENT_TIME - $FILE_TIME) / 86400 ))
            
            if [ $DAYS_OLD -gt 7 ]; then
                print_info "删除旧备份: $(basename $file)"
                rm "$file"
                ((DELETED++))
            fi
        fi
    done
    
    if [ $DELETED -gt 0 ]; then
        print_info "已删除 $DELETED 个旧备份文件"
    else
        print_info "没有需要清理的旧备份"
    fi
}

# 函数：显示备份列表
list_backups() {
    print_info "现有备份列表："
    echo "----------------------------------------"
    
    if [ -d "$BACKUP_DIR" ]; then
        COUNT=0
        for file in "$BACKUP_DIR"/termux_backup_*.tar.gz; do
            if [ -f "$file" ]; then
                SIZE=$(du -h "$file" | cut -f1)
                DATE=$(stat -c %y "$file" 2>/dev/null | cut -d' ' -f1 || stat -f %Sm "$file" 2>/dev/null)
                echo "  $(basename $file) - $SIZE - $DATE"
                ((COUNT++))
            fi
        done
        
        if [ $COUNT -eq 0 ]; then
            print_warn "没有找到备份文件"
        else
            print_info "共找到 $COUNT 个备份文件"
        fi
    else
        print_warn "备份目录不存在: $BACKUP_DIR"
    fi
}

# 主程序
main() {
    echo "========================================"
    echo "    Termux 系统备份工具"
    echo "========================================"
    echo ""
    
    # 检查参数
    case "$1" in
        "list")
            list_backups
            ;;
        "cleanup")
            cleanup_old_backups
            ;;
        *)
            perform_backup
            echo ""
            cleanup_old_backups
            echo ""
            list_backups
            ;;
    esac
    
    echo ""
    echo "========================================"
    print_info "操作完成！"
    echo "========================================"
}

# 执行主程序
main "$@"
