from pathlib import Path

DATA_DIR = Path("data")
DATA_DIR.mkdir(exist_ok=True)
def generate_pattern_file(path: Path, size: int) -> None:
    pattern = bytes(range(256))
    with path.open("wb") as file:
        full_blocks = size // len(pattern)
        remainder = size % len(pattern)
        for _ in range(full_blocks):
            file.write(pattern)
        if remainder:
            file.write(pattern[:remainder])

FILES = {
    "empty.bin": 0,
    "one_byte.bin": 1,
    "small.bin": 31,
    "4k.bin": 4 * 1024,
    "4k_plus_1.bin": 4 * 1024 + 1,
    "64k.bin": 64 * 1024,
    "1m.bin": 1024 * 1024,
    "100m.bin": 100 * 1024 * 1024,
}

for name, size in FILES.items():
    path = DATA_DIR / name
    generate_pattern_file(path, size)
    print(f"generated {path} ({size} bytes)")