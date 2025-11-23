import os
import subprocess
import sys
from itertools import product

def main():
    base_dirs = [
        os.path.join('app'),
        os.path.join('engine'),
        os.path.join('engine', 'test')
    ]

    extensions = {'.cpp', '.c', '.h', '.hpp', '.cc', '.cxx'}

    project_root = os.path.dirname(os.path.abspath(__file__))

    subdirs = ["src", "include"]

    for base, subdir in product(base_dirs, subdirs):
        abs_dir = os.path.join(project_root, base, subdir)
        print("Formatting directory:", abs_dir)
        for root, dirs, files in os.walk(abs_dir):            
            for file in files:
                ext = os.path.splitext(file)[1]
                if ext in extensions:
                    file_path = os.path.join(root, file)
                    try:
                        subprocess.run(['clang-format', '-i', '-style=file', file_path], check=True)
                    except subprocess.CalledProcessError as e:
                        print(f"Error formatting {file}: {e}")
                    except FileNotFoundError:
                        print("Error: clang-format not found. Please install it.")
                        sys.exit(1)

if __name__ == "__main__":
    main()
