import sys
import os
import json
import subprocess

def main():
    if len(sys.argv) != 3:
        sys.exit(1)

    dir_path = sys.argv[1]
    file_path = sys.argv[2]
    exec_file = os.path.join(dir_path, "build", "VSA.out")
    real_file = os.path.join(dir_path, "tests", "test.txt")
    ref_file = file_path + ".json"

    # Запуск исполняемого файла и перенаправление ввода/вывода
    with open(real_file, "w") as real_f:
        subprocess.run([exec_file, "json"], stdin=open(file_path, "r"), stdout=real_f, stderr=subprocess.PIPE)

    # Чтение содержимого файлов
    with open(real_file, "r") as real_f, open(ref_file, "r") as ref_f:
        real_content = real_f.read()
        ref_content = ref_f.read()

    # Удаление временного файла
    os.remove(real_file)

    # Сравнение JSON
    ans = (json.loads(real_content) != json.loads(ref_content))
    sys.exit(int(ans))

if __name__ == "__main__":
    main()
