from glob import glob

def globr(patterns: list[str]) -> list[str]:
    files: set[str] = set()
    for arg in patterns:
        for filepath in glob(arg, recursive=True):
            files.add(filepath)
    return list(files)
