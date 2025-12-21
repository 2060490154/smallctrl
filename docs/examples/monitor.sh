#!/bin/bash
# 系统监控脚本
# 显示系统状态、资源使用情况和进程信息

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 获取系统信息
get_system_info() {
    echo -e "${CYAN}╔════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║       系统信息 / System Info          ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════╝${NC}"
    
    # 主机名
    echo -e "${GREEN}主机名:${NC} $(hostname)"
    
    # 系统版本
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo -e "${GREEN}系统:${NC} $PRETTY_NAME"
    fi
    
    # 内核版本
    echo -e "${GREEN}内核:${NC} $(uname -r)"
    
    # 运行时间
    UPTIME=$(uptime -p 2>/dev/null || echo "N/A")
    echo -e "${GREEN}运行时间:${NC} $UPTIME"
    
    echo ""
}

# CPU 信息
get_cpu_info() {
    echo -e "${CYAN}╔════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║         CPU 信息 / CPU Info           ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════╝${NC}"
    
    # CPU 型号
    if [ -f /proc/cpuinfo ]; then
        CPU_MODEL=$(grep -m 1 "model name" /proc/cpuinfo | cut -d':' -f2 | xargs)
        if [ -z "$CPU_MODEL" ]; then
            CPU_MODEL=$(grep -m 1 "Hardware" /proc/cpuinfo | cut -d':' -f2 | xargs)
        fi
        echo -e "${GREEN}CPU 型号:${NC} ${CPU_MODEL:-N/A}"
    fi
    
    # CPU 核心数
    CPU_CORES=$(grep -c ^processor /proc/cpuinfo 2>/dev/null || echo "N/A")
    echo -e "${GREEN}CPU 核心:${NC} $CPU_CORES"
    
    # CPU 使用率
    if command -v top &> /dev/null; then
        CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | sed "s/.*, *\([0-9.]*\)%* id.*/\1/" | awk '{print 100 - $1}')
        if [ -z "$CPU_USAGE" ]; then
            CPU_USAGE="N/A"
        else
            # 根据使用率着色
            if (( $(echo "$CPU_USAGE > 80" | bc -l 2>/dev/null || echo 0) )); then
                COLOR=$RED
            elif (( $(echo "$CPU_USAGE > 50" | bc -l 2>/dev/null || echo 0) )); then
                COLOR=$YELLOW
            else
                COLOR=$GREEN
            fi
            CPU_USAGE="${COLOR}${CPU_USAGE}%${NC}"
        fi
        echo -e "${GREEN}CPU 使用率:${NC} $CPU_USAGE"
    fi
    
    echo ""
}

# 内存信息
get_memory_info() {
    echo -e "${CYAN}╔════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║       内存信息 / Memory Info          ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════╝${NC}"
    
    if command -v free &> /dev/null; then
        # 获取内存信息
        MEM_INFO=$(free -h | grep "Mem:")
        TOTAL=$(echo $MEM_INFO | awk '{print $2}')
        USED=$(echo $MEM_INFO | awk '{print $3}')
        FREE=$(echo $MEM_INFO | awk '{print $4}')
        
        echo -e "${GREEN}总内存:${NC} $TOTAL"
        echo -e "${GREEN}已使用:${NC} $USED"
        echo -e "${GREEN}可用:${NC} $FREE"
        
        # 计算使用百分比
        TOTAL_KB=$(free | grep "Mem:" | awk '{print $2}')
        USED_KB=$(free | grep "Mem:" | awk '{print $3}')
        
        if [ "$TOTAL_KB" -gt 0 ]; then
            PERCENT=$((USED_KB * 100 / TOTAL_KB))
            
            # 根据使用率着色
            if [ $PERCENT -gt 80 ]; then
                COLOR=$RED
            elif [ $PERCENT -gt 60 ]; then
                COLOR=$YELLOW
            else
                COLOR=$GREEN
            fi
            
            echo -e "${GREEN}使用率:${NC} ${COLOR}${PERCENT}%${NC}"
            
            # 绘制进度条
            BAR_LENGTH=30
            FILLED=$((PERCENT * BAR_LENGTH / 100))
            printf "${GREEN}进度:${NC} ["
            for ((i=0; i<BAR_LENGTH; i++)); do
                if [ $i -lt $FILLED ]; then
                    printf "${COLOR}█${NC}"
                else
                    printf "░"
                fi
            done
            printf "] ${COLOR}${PERCENT}%%${NC}\n"
        fi
    fi
    
    echo ""
}

# 磁盘信息
get_disk_info() {
    echo -e "${CYAN}╔════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║       磁盘信息 / Disk Info            ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════╝${NC}"
    
    if command -v df &> /dev/null; then
        # 主目录磁盘使用
        DISK_INFO=$(df -h $HOME | tail -1)
        SIZE=$(echo $DISK_INFO | awk '{print $2}')
        USED=$(echo $DISK_INFO | awk '{print $3}')
        AVAIL=$(echo $DISK_INFO | awk '{print $4}')
        PERCENT=$(echo $DISK_INFO | awk '{print $5}' | sed 's/%//')
        
        echo -e "${GREEN}总空间:${NC} $SIZE"
        echo -e "${GREEN}已使用:${NC} $USED"
        echo -e "${GREEN}可用:${NC} $AVAIL"
        
        # 根据使用率着色
        if [ "$PERCENT" -gt 80 ]; then
            COLOR=$RED
        elif [ "$PERCENT" -gt 60 ]; then
            COLOR=$YELLOW
        else
            COLOR=$GREEN
        fi
        
        echo -e "${GREEN}使用率:${NC} ${COLOR}${PERCENT}%${NC}"
    fi
    
    echo ""
}

# 进程信息
get_process_info() {
    echo -e "${CYAN}╔════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║       进程信息 / Process Info         ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════╝${NC}"
    
    # 进程总数
    TOTAL_PROC=$(ps aux | wc -l)
    echo -e "${GREEN}运行进程:${NC} $((TOTAL_PROC - 1))"
    
    # 用户进程数
    USER_PROC=$(ps aux | grep "^$(whoami)" | wc -l)
    echo -e "${GREEN}用户进程:${NC} $((USER_PROC - 1))"
    
    echo ""
    echo -e "${YELLOW}Top 5 CPU 使用:${NC}"
    ps aux --sort=-%cpu | head -6 | tail -5 | awk '{printf "  %-20s %5s%%  %s\n", $11, $3, $2}'
    
    echo ""
    echo -e "${YELLOW}Top 5 内存使用:${NC}"
    ps aux --sort=-%mem | head -6 | tail -5 | awk '{printf "  %-20s %5s%%  %s\n", $11, $4, $2}'
    
    echo ""
}

# 网络信息
get_network_info() {
    echo -e "${CYAN}╔════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║       网络信息 / Network Info         ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════╝${NC}"
    
    # IP 地址
    if command -v ip &> /dev/null; then
        IP_ADDR=$(ip addr show | grep "inet " | grep -v "127.0.0.1" | awk '{print $2}' | cut -d'/' -f1 | head -1)
        if [ -n "$IP_ADDR" ]; then
            echo -e "${GREEN}IP 地址:${NC} $IP_ADDR"
        fi
    fi
    
    # 网络连接数
    if command -v ss &> /dev/null; then
        ESTABLISHED=$(ss -t | grep ESTAB | wc -l)
        LISTENING=$(ss -tln | grep LISTEN | wc -l)
        echo -e "${GREEN}活动连接:${NC} $ESTABLISHED"
        echo -e "${GREEN}监听端口:${NC} $LISTENING"
    elif command -v netstat &> /dev/null; then
        ESTABLISHED=$(netstat -t | grep ESTABLISHED | wc -l)
        LISTENING=$(netstat -tln | grep LISTEN | wc -l)
        echo -e "${GREEN}活动连接:${NC} $ESTABLISHED"
        echo -e "${GREEN}监听端口:${NC} $LISTENING"
    fi
    
    echo ""
}

# 快速概览
quick_overview() {
    clear
    echo -e "${PURPLE}╔══════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                                                  ║${NC}"
    echo -e "${PURPLE}║        Termux 系统监控工具                       ║${NC}"
    echo -e "${PURPLE}║        System Monitor for Termux                 ║${NC}"
    echo -e "${PURPLE}║                                                  ║${NC}"
    echo -e "${PURPLE}╚══════════════════════════════════════════════════╝${NC}"
    echo ""
    
    get_system_info
    get_cpu_info
    get_memory_info
    get_disk_info
    get_process_info
    get_network_info
    
    echo -e "${CYAN}════════════════════════════════════════${NC}"
    echo -e "${GREEN}刷新时间:${NC} $(date '+%Y-%m-%d %H:%M:%S')"
    echo -e "${CYAN}════════════════════════════════════════${NC}"
}

# 持续监控模式
continuous_monitor() {
    while true; do
        quick_overview
        echo ""
        echo -e "${YELLOW}按 Ctrl+C 退出监控${NC}"
        sleep 5
    done
}

# 主函数
main() {
    case "$1" in
        "-c"|"--continuous")
            continuous_monitor
            ;;
        "-h"|"--help")
            echo "用法: $0 [选项]"
            echo ""
            echo "选项:"
            echo "  -c, --continuous    持续监控模式（每5秒刷新）"
            echo "  -h, --help          显示帮助信息"
            echo ""
            echo "无参数时显示一次性监控信息"
            ;;
        *)
            quick_overview
            ;;
    esac
}

# 运行主函数
main "$@"
