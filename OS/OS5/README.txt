БЫСТРО: bash run_all.sh   (нужны gcc и strace: sudo apt install gcc strace)

ПО ОДНОМУ:
  gcc -Wall -Wextra lab5_commented.c -o lab5
  gcc -Wall -Wextra lab5_part1.c -o lab5_part1
  cat file1 | ./lab5_part1                      # скриншот 1
  cat passwd_test | ./lab5 2>/dev/null          # скриншот 2 (stdout)
  cat passwd_test | ./lab5 2>&1 >/dev/null      # скриншот 2 (stderr)
  tail -n 3 passwd_test | ./lab5 2>/dev/null    # скриншот 3
  cat uid_names.txt; cat gid_names.txt          # скриншот 4
  strace -o trace.txt ./lab5 < passwd_small >/dev/null 2>&1
  cat trace.txt                                 # скриншот 5
