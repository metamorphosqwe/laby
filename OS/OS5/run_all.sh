#!/bin/bash
# Запуск всех команд, по которым сделаны скриншоты в отчёте.
# Использование: bash run_all.sh   (в папке со всеми файлами)
set -u
gcc -Wall -Wextra lab5.c -o lab5 || exit 1
gcc -Wall -Wextra lab5_part1.c -o lab5_part1 || exit 1
run() { echo "user@ubuntu:~\$ $1"; bash -c "$1"; }
echo "=== 1. Часть I ==="
run "cat file1 | ./lab5_part1"
run "cat file1 | sort | head -3 | ./lab5_part1"
echo "=== 2. Часть II: stdout и stderr ==="
run "cat passwd_test | ./lab5 2>/dev/null"
run "cat passwd_test | ./lab5 2>&1 >/dev/null"
echo "=== 3. Часть II: конвейеры ==="
run "sort -t: -k3 -n -r passwd_test | head -4 | ./lab5 2>&1 >/dev/null"
run "tail -n 3 passwd_test | ./lab5 2>/dev/null"
run "cat passwd_test | ./lab5 2>/dev/null | sort -r | head -n 2"
echo "=== 4. Часть III: файлы ==="
run "cat passwd_test | ./lab5 >/dev/null 2>&1"
run "ls -l uid_names.txt gid_names.txt"
run "cat uid_names.txt"
run "cat gid_names.txt"
echo "=== 5. strace ==="
run "strace -o trace.txt ./lab5 < passwd_small >/dev/null 2>&1"
run "sed -n '/read(0/,\$p' trace.txt"
