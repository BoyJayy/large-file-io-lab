from pathlib import Path
import argparse


DATA_DIR = Path("data")

KiB = 1024
MiB = 1024*KiB
GiB = 1024*MiB
DEV_FILES = {
    "empty.bin": 0,
    "one_byte.bin": 1,
    "small.bin": 31,
    "4KiB.bin": 4 * KiB,
    "4KiB_plus_1.bin": 4 * KiB + 1,
    "64KiB.bin": 64 * KiB,
    "1MiB.bin": 1 * MiB,
    "100MiB.bin": 100 * MiB,
}
BENCH_FILES = {
    "256MiB.bin": 256 * MiB,
    "1GiB.bin": 1 * GiB,
    "4GiB.bin": 4 * GiB,
}

def generate_pattern_file(path: Path, size: int) -> None:
    # 1 MiB deterministic block:
    # 00 01 02 ... FE FF repeated
    pattern = bytes(range(256))
    chunk = pattern * (MiB // len(pattern))

    with path.open("wb") as file:
        remaining = size

        while remaining > 0:
            n = min(remaining, len(chunk))
            file.write(chunk[:n])
            remaining -= n


def generate(files: dict[str, int], force: bool) -> None:
    DATA_DIR.mkdir(exist_ok=True)

    for name, size in files.items():
        path = DATA_DIR / name

        if path.exists() and path.stat().st_size == size and not force:
            print(f"skip      {path} ({size} bytes)")
            continue

        print(f"generating {path} ({size} bytes)")
        generate_pattern_file(path, size)
        print(f"done       {path}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--profile",
        choices=["dev", "bench", "all"],
        default="dev",
        help="dataset profile to generate",
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="regenerate files even if they already exist",
    )
    args = parser.parse_args()
    if args.profile == "dev":
        files = DEV_FILES
    elif args.profile == "bench":
        files = BENCH_FILES
    else:
        files = DEV_FILES | BENCH_FILES
    generate(files, args.force)


if __name__ == "__main__":
    main()