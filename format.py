import argparse
import os
import subprocess
import sys
from itertools import product

EXTENSIONS = {".cpp", ".c", ".h", ".hpp", ".cc", ".cxx"}


def format_file(file_path, check):
    command = ["clang-format", "-style=file"]

    if check:
        command.append("--dry-run")
        command.append("--Werror")
    else:
        command.append("-i")

    command.append(file_path)

    try:
        subprocess.run(command, check=True)
        return True
    except subprocess.CalledProcessError as e:
        if check:
            print(f"Formatting check failed: {file_path}")
        else:
            print(f"Error formatting {file_path}: {e}")
        return False
    except FileNotFoundError:
        print("Error: clang-format not found. Please install it.")
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(description="Format C/C++ source files.")
    parser.add_argument(
        "paths",
        nargs="*",
        help="Files to format/check. If omitted, format/check the default project directories.",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="Check formatting without modifying files.",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Prints the current directory being checked",
    )

    args = parser.parse_args()

    project_root = os.path.dirname(os.path.abspath(__file__))

    if args.paths:
        files = [
            os.path.abspath(path)
            for path in args.paths
            if os.path.splitext(path)[1] in EXTENSIONS
        ]
    else:
        base_dirs = [
            "app",
            "engine",
            os.path.join("engine", "test", "app"),
        ]
        subdirs = ["src", "include"]

        files = []

        for base, subdir in product(base_dirs, subdirs):
            abs_dir = os.path.join(project_root, base, subdir)

            if not os.path.isdir(abs_dir):
                continue

            if args.verbose:
                print(
                    f"{'Checking' if args.check else 'Formatting'} directory: {abs_dir}"
                )

            for root, dirs, filenames in os.walk(abs_dir):
                for filename in filenames:
                    if os.path.splitext(filename)[1] in EXTENSIONS:
                        files.append(os.path.join(root, filename))

    success = True

    for file_path in files:
        if args.verbose:
            print(f"{'Checking' if args.check else 'Formatting'} file: {file_path}")

        if not format_file(file_path, args.check):
            success = False

    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
