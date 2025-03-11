#!/bin/bash

# 定义变量
LOCAL_DIR="./{*,.git,.gitignore}"
REMOTE_USER="wangyidi"
REMOTE_HOST="162.105.89.151"
REMOTE_PORT="2020"
REMOTE_DIR="/home/wangyidi/boom_perf/src/"
SSH_CMD="ssh -p $REMOTE_PORT"

# 显示使用说明
function show_usage {
    echo "用法: $0 [选项]"
    echo "选项:"
    echo "  -u, --upload    将本地文件同步到服务器（默认）"
    echo "  -d, --download  将服务器文件同步到本地"
    echo "  -h, --help      显示此帮助信息"
    exit 1
}

# 默认模式为上传
MODE="upload"

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case "$1" in
        -u|--upload)
            MODE="upload"
            shift
            ;;
        -d|--download)
            MODE="download"
            shift
            ;;
        -h|--help)
            show_usage
            ;;
        *)
            echo "错误: 未知选项 $1"
            show_usage
            ;;
    esac
done

# 根据模式执行相应的同步操作
if [[ "$MODE" == "upload" ]]; then
    echo "正在将本地文件同步到服务器..."
    rsync -av -e "$SSH_CMD" ./{*,.git,.gitignore} $REMOTE_USER@$REMOTE_HOST:$REMOTE_DIR
    echo "上传完成！"
elif [[ "$MODE" == "download" ]]; then
    echo "正在将服务器文件同步到本地..."
    rsync -av -e "$SSH_CMD" $REMOTE_USER@$REMOTE_HOST:$REMOTE_DIR .
    echo "下载完成！"
fi