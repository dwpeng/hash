import os
import sys

language_map = {
    "c": "gcc",
    "cpp": "g++",
    "cc": "g++",
}


def complie_file(src: str, exe_name: str, flags: list = [], deps: list = []):
    if not os.path.isfile(src):
        print(f"Error: {src} is not a file.")
        sys.exit(1)

    if not os.path.isdir(output_dir):
        print(f"Error: {output_dir} is not a directory.")
        sys.exit(1)

    language = src.split(".")[-1]
    compiler = language_map.get(language, None)
    if compiler is None:
        print(f"Error: {language} is not supported.")
        sys.exit(1)

    deps_artifacts = []
    for dep in deps:
        compiler = language_map.get(dep.split(".")[-1], None)
        if compiler is None:
            print(f"Error: {dep} is not supported.")
            sys.exit(1)
        source_name = dep.split("/")[-1]
        source_name = source_name.split(".")[0]
        command = [compiler, dep, "-c", "-o", source_name + ".o"] + flags
        os.system(" ".join(command))
        deps_artifacts.append(source_name + ".o")

    compiler = language_map.get(language, None)
    command = [compiler, src, "-o", exe_name] + deps_artifacts + flags
    os.system(" ".join(command))
    for dep in deps_artifacts:
        if os.path.exists(dep):
            os.remove(dep)


if __name__ == "__main__":
    deps = ["thpool/thpool.c"]
    test_dir = "test"
    output_dir = "."
    flags = ["-Wall", "-g", "-O3", "-std=c11", "-I.", "-I./thpool"]
    sources = os.listdir(test_dir)
    for src in sources:
        exe_name = src.split(".")[0]
        exe_name = os.path.join(output_dir, exe_name)
        if os.path.exists(exe_name):
            os.remove(exe_name)
        try:
            complie_file(os.path.join(test_dir, src), exe_name, flags, deps)
        except Exception as e:
            print(f"Error: complie {src} failed. {e}")
        else:
            print(f"Complie \033[1;32m{src}\033[0m successfully.")
