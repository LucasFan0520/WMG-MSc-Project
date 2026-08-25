#!/usr/bin/env python3

#Enter the following command in the terminal: python "directory containing rename_c_from_header.py" "directory of the target C file" --apply


from __future__ import annotations

import argparse
import re
import sys
from collections import Counter
from pathlib import Path


ID_PATTERN = re.compile(r"(?<![A-Za-z0-9_])F(?P<num>\d{3})(?![A-Za-z0-9_])", re.IGNORECASE)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Rename .c files to F001.c style names found on the first line."
    )
    parser.add_argument(
        "path",
        nargs="?",
        default=".",
        type=Path,
        help="Directory containing .c files. Default: current directory.",
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="Actually rename files. Without this flag, only preview the rename plan.",
    )
    parser.add_argument(
        "--recursive",
        action="store_true",
        help="Search for .c files recursively.",
    )
    parser.add_argument(
        "--min",
        dest="min_id",
        type=int,
        default=1,
        help="Minimum allowed numeric id. Default: 1.",
    )
    parser.add_argument(
        "--max",
        dest="max_id",
        type=int,
        default=999,
        help="Maximum allowed numeric id. Default: 999.",
    )
    return parser.parse_args()


def read_first_line(path: Path) -> str:
    with path.open("r", encoding="utf-8-sig", errors="replace") as handle:
        return handle.readline().rstrip("\r\n")


def find_id(line: str, min_id: int, max_id: int) -> str | None:
    match = ID_PATTERN.search(line)
    if not match:
        return None
    value = int(match.group("num"))
    if min_id <= value <= max_id:
        return f"F{value:03d}"
    return None


def build_plan(root: Path, recursive: bool, min_id: int, max_id: int) -> tuple[list[tuple[Path, Path]], list[str]]:
    pattern = "**/*.c" if recursive else "*.c"
    files = sorted(path for path in root.glob(pattern) if path.is_file())
    plan: list[tuple[Path, Path]] = []
    warnings: list[str] = []

    for path in files:
        first_line = read_first_line(path)
        file_id = find_id(first_line, min_id, max_id)
        if file_id is None:
            warnings.append(f"Cannot find Fxxx on line 1: {path}")
            continue

        target = path.with_name(f"{file_id}.c")
        if path.resolve() == target.resolve():
            continue
        plan.append((path, target))

    return plan, warnings


def validate_plan(plan: list[tuple[Path, Path]]) -> list[str]:
    errors: list[str] = []
    target_counts = Counter(target.resolve() for _, target in plan)
    duplicate_targets = [target for target, count in target_counts.items() if count > 1]
    for target in duplicate_targets:
        errors.append(f"Duplicate target name in rename plan: {target}")

    sources = {source.resolve() for source, _ in plan}
    for source, target in plan:
        if target.exists() and target.resolve() not in sources:
            errors.append(f"Target already exists: {target} (from {source.name})")

    return errors


def print_plan(plan: list[tuple[Path, Path]], warnings: list[str]) -> None:
    if warnings:
        print("Warnings:")
        for warning in warnings:
            print(f"  - {warning}")
        print()

    if not plan:
        print("No files need renaming.")
        return

    print("Rename plan:")
    for source, target in plan:
        print(f"  {source.name} -> {target.name}")


def apply_plan(plan: list[tuple[Path, Path]]) -> int:
    # Rename through temporary names first so swaps/cycles cannot collide.
    temp_pairs: list[tuple[Path, Path, Path]] = []
    for index, (source, target) in enumerate(plan, start=1):
        temp = source.with_name(f".__rename_tmp_{index:04d}_{source.name}")
        if temp.exists():
            raise RuntimeError(f"Temporary file already exists: {temp}")
        source.rename(temp)
        temp_pairs.append((temp, source, target))

    try:
        renamed_count = 0
        for temp, _source, target in temp_pairs:
            temp.rename(target)
            renamed_count += 1
            print(f"Renamed: {target.name}")
        return renamed_count
    except Exception:
        # Best effort rollback for files that have not yet reached targets.
        for temp, source, _target in temp_pairs:
            if temp.exists() and not source.exists():
                temp.rename(source)
        raise


def main() -> int:
    args = parse_args()
    root = args.path.resolve()
    if not root.exists() or not root.is_dir():
        print(f"Directory not found: {root}", file=sys.stderr)
        return 2
    if args.min_id > args.max_id:
        print("--min cannot be greater than --max", file=sys.stderr)
        return 2

    plan, warnings = build_plan(root, args.recursive, args.min_id, args.max_id)
    print_plan(plan, warnings)

    errors = validate_plan(plan)
    if errors:
        print("\nErrors:")
        for error in errors:
            print(f"  - {error}")
        print("\nRename stopped.")
        return 1

    if not args.apply:
        print("\nPreview only. Add --apply to perform the rename.")
        return 0

    renamed_count = apply_plan(plan)
    print(f"Done. Renamed {renamed_count} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
