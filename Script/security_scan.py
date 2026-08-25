#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass
from pathlib import Path


SMOKE_TESTS = [
    ("empty_stdin", [], ""),
    ("end_only", [], "END\n"),
    ("unknown_command", [], "UNKNOWN x y z\nEND\n"),
    ("long_line", [], "A" * 8192 + "\nEND\n"),
    ("format_tokens", [], "%x %x %x %n %s\nEND\n"),
    ("many_lines", [], ("PING alpha beta gamma\n" * 1024) + "END\n"),
    ("special_chars", [], "\"'\\`$;&|<>../..\\\\..\\\\\nEND\n"),
]


SEVERITY_WEIGHT = {
    "high": 10,
    "medium": 4,
    "low": 1,
    "info": 0,
}


@dataclass
class Finding:
    file: str
    line: int
    severity: str
    confidence: str
    category: str
    check_id: str
    message: str
    evidence: str


@dataclass
class FunctionCall:
    name: str
    args: list[str]
    line: int
    text: str
    start: int
    end: int


@dataclass
class FunctionDef:
    name: str
    params: list[str]
    body: str
    body_start: int
    body_end: int


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Strict conservative scanner for generated C files.")
    parser.add_argument("--src", required=True, type=Path, help="Directory containing .c files.")
    parser.add_argument("--out", type=Path, default=Path("security_results_fair"), help="Output directory.")
    parser.add_argument("--recursive", action="store_true", help="Scan .c files recursively.")
    parser.add_argument("--timeout", type=float, default=10.0, help="Compile/run timeout in seconds.")
    parser.add_argument("--std", default="gnu11", help="Compiler standard for gcc/clang.")
    parser.add_argument(
        "--with-compile",
        action="store_true",
        help="Compatibility option. Compile checks are enabled by default.",
    )
    parser.add_argument(
        "--with-smoke",
        action="store_true",
        help="Compatibility option. Smoke tests are enabled by default.",
    )
    parser.add_argument(
        "--skip-smoke",
        action="store_true",
        help="Run static checks and compile checks, but skip runtime smoke tests.",
    )
    parser.add_argument(
        "--static-only",
        action="store_true",
        help="Run only static security checks; skip compile checks and runtime smoke tests.",
    )
    parser.add_argument(
        "--skip-aux-static",
        action="store_true",
        help="Skip auxiliary Cppcheck and Clang Static Analyzer outputs.",
    )
    parser.add_argument("--keep-build", action="store_true", help="Keep compiled binaries.")
    return parser.parse_args()


def find_c_files(src: Path, recursive: bool) -> list[Path]:
    pattern = "**/*.c" if recursive else "*.c"
    return sorted(path for path in src.glob(pattern) if path.is_file())


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def line_text(source: str, line: int) -> str:
    lines = source.splitlines()
    if 1 <= line <= len(lines):
        return lines[line - 1].strip()[:240]
    return ""


def strip_comments_preserve_lines(source: str) -> str:
    source = re.sub(
        r"/\*.*?\*/",
        lambda match: "\n" * match.group(0).count("\n"),
        source,
        flags=re.S,
    )
    return re.sub(r"//.*", "", source)


def mask_literals_preserve_lines(code: str) -> str:
    chars = list(code)
    quote: str | None = None
    escape = False
    for index, char in enumerate(chars):
        if quote:
            if char == "\n":
                quote = None
                escape = False
                continue
            if escape:
                chars[index] = " "
                escape = False
                continue
            if char == "\\":
                chars[index] = " "
                escape = True
                continue
            if char == quote:
                chars[index] = " "
                quote = None
                continue
            chars[index] = " "
        elif char in {'"', "'"}:
            chars[index] = " "
            quote = char
    return "".join(chars)


def find_matching_delimiter(code: str, open_index: int, opener: str, closer: str) -> int | None:
    depth = 0
    quote: str | None = None
    escape = False
    for index in range(open_index, len(code)):
        char = code[index]
        if quote:
            if escape:
                escape = False
            elif char == "\\":
                escape = True
            elif char == quote:
                quote = None
            continue
        if char in {'"', "'"}:
            quote = char
        elif char == opener:
            depth += 1
        elif char == closer:
            depth -= 1
            if depth == 0:
                return index
    return None


def split_arguments(args_text: str) -> list[str]:
    args: list[str] = []
    start = 0
    paren = bracket = brace = 0
    quote: str | None = None
    escape = False
    for index, char in enumerate(args_text):
        if quote:
            if escape:
                escape = False
            elif char == "\\":
                escape = True
            elif char == quote:
                quote = None
            continue
        if char in {'"', "'"}:
            quote = char
        elif char == "(":
            paren += 1
        elif char == ")":
            paren = max(paren - 1, 0)
        elif char == "[":
            bracket += 1
        elif char == "]":
            bracket = max(bracket - 1, 0)
        elif char == "{":
            brace += 1
        elif char == "}":
            brace = max(brace - 1, 0)
        elif char == "," and paren == bracket == brace == 0:
            args.append(args_text[start:index].strip())
            start = index + 1
    tail = args_text[start:].strip()
    if tail or args_text.strip():
        args.append(tail)
    return args


def iter_function_calls(code: str, names: set[str] | list[str]) -> list[FunctionCall]:
    if not names:
        return []
    masked = mask_literals_preserve_lines(code)
    pattern = re.compile(r"\b(" + "|".join(re.escape(name) for name in sorted(names)) + r")\s*\(")
    calls: list[FunctionCall] = []
    for match in pattern.finditer(masked):
        name = match.group(1)
        open_index = masked.find("(", match.start())
        close_index = find_matching_delimiter(code, open_index, "(", ")")
        if close_index is None:
            continue
        args_text = code[open_index + 1 : close_index]
        calls.append(
            FunctionCall(
                name=name,
                args=split_arguments(args_text),
                line=code[: match.start()].count("\n") + 1,
                text=code[match.start() : close_index + 1],
                start=match.start(),
                end=close_index + 1,
            )
        )
    return calls


def extract_c_string_literal(expr: str) -> str | None:
    text = expr.strip()
    literal = re.compile(r'(?:u8|[LuU])?"((?:\\.|[^"\\])*)"')
    pieces: list[str] = []
    cursor = 0
    for match in literal.finditer(text):
        if text[cursor : match.start()].strip():
            return None
        pieces.append(match.group(1))
        cursor = match.end()
    if text[cursor:].strip() or not pieces:
        return None
    return "".join(pieces)


def c_string_literal_length(expr: str) -> int | None:
    literal = extract_c_string_literal(expr)
    if literal is None:
        return None
    length = 0
    index = 0
    while index < len(literal):
        if literal[index] != "\\":
            length += 1
            index += 1
            continue
        index += 1
        if index >= len(literal):
            length += 1
            break
        if literal[index] in "xX":
            index += 1
            while index < len(literal) and literal[index] in "0123456789abcdefABCDEF":
                index += 1
        elif literal[index] in "01234567":
            count = 0
            while index < len(literal) and literal[index] in "01234567" and count < 3:
                index += 1
                count += 1
        else:
            index += 1
        length += 1
    return length


def strip_outer_parens(expr: str) -> str:
    text = expr.strip()
    while text.startswith("(") and text.endswith(")"):
        close = find_matching_delimiter(text, 0, "(", ")")
        if close != len(text) - 1:
            break
        text = text[1:-1].strip()
    return text


def compact_expr(expr: str) -> str:
    return re.sub(r"\s+", "", strip_outer_parens(expr))


def simple_identifier_expr(expr: str) -> str | None:
    text = compact_expr(expr)
    return text if re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", text) else None


def int_literal_value(expr: str) -> int | None:
    text = compact_expr(expr)
    return int(text, 0) if re.fullmatch(r"0[xX][0-9A-Fa-f]+|[0-9]+", text) else None


def integer_value_before(code: str, expr: str, pos: int) -> int | None:
    literal = int_literal_value(expr)
    if literal is not None:
        return literal
    name = compact_expr(expr)
    if name in {"sizeof(char)", "sizeof(signedchar)", "sizeof(unsignedchar)", "sizeofchar"}:
        return 1
    if name.startswith("sizeof(") and name.endswith(")"):
        target = name[len("sizeof(") : -1]
        capacity = expression_capacity_before(code, target, pos)
        if capacity is not None:
            return capacity
    sizeof_match = re.fullmatch(r"sizeof\(?\s*(?P<target>[A-Za-z_][A-Za-z0-9_]*)\s*\)?", name)
    if sizeof_match:
        if sizeof_match.group("target") in {"char", "signedchar", "unsignedchar"}:
            return 1
        return char_array_capacity_before(code, sizeof_match.group("target"), pos)
    arithmetic_match = re.fullmatch(r"(?P<left>.+)(?P<op>[+-])(?P<right>0[xX][0-9A-Fa-f]+|[0-9]+)", name)
    if arithmetic_match:
        left = integer_value_before(code, arithmetic_match.group("left"), pos)
        right = int(arithmetic_match.group("right"), 0)
        if left is not None:
            return left + right if arithmetic_match.group("op") == "+" else left - right
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", name):
        return None
    pattern = rf"(?m)^\s*#\s*define\s+{re.escape(name)}\s+(?P<value>0[xX][0-9A-Fa-f]+|[0-9]+)\b"
    best: int | None = None
    for match in re.finditer(pattern, code[:pos]):
        best = int(match.group("value"), 0)
    return best


def char_array_capacity_before(code: str, name: str, pos: int) -> int | None:
    best: int | None = None
    declaration = re.compile(r"\b(?:unsigned\s+char|signed\s+char|char)\s+(?P<decls>[^;]+);")
    for match in declaration.finditer(code[:pos]):
        for part in split_arguments(match.group("decls")):
            item = re.sub(r"=.*$", "", part.strip()).strip()
            item_match = re.search(rf"(?:\*+\s*)?\b{re.escape(name)}\s*\[\s*(?P<size>[^\]]+)\s*\]", item)
            if not item_match:
                continue
            size = integer_value_before(code, item_match.group("size"), pos)
            if size is not None:
                best = size
    return best


def declarator_name(declarator: str) -> str | None:
    item = re.sub(r"=.*$", "", declarator.strip()).strip()
    item = re.sub(r"\[[^\]]*\]", "", item)
    names = re.findall(r"[A-Za-z_][A-Za-z0-9_]*", item)
    if not names:
        return None
    candidate = names[-1]
    if candidate in {"const", "volatile", "struct", "char", "int", "long", "short", "unsigned", "signed", "void"}:
        return None
    return candidate


def parse_char_array_fields(code: str, body: str, pos: int) -> dict[str, int]:
    fields: dict[str, int] = {}
    declaration = re.compile(r"\b(?:unsigned\s+char|signed\s+char|char)\s+(?P<decls>[^;]+);")
    for match in declaration.finditer(body):
        for part in split_arguments(match.group("decls")):
            item = re.sub(r"=.*$", "", part.strip()).strip()
            field_match = re.search(r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*\[\s*(?P<size>[^\]]+)\s*\]", item)
            if not field_match:
                continue
            size = integer_value_before(code, field_match.group("size"), pos)
            if size is not None:
                fields[field_match.group("name")] = size
    return fields


def struct_array_fields_before(code: str, pos: int) -> dict[str, dict[str, int]]:
    type_fields: dict[str, dict[str, int]] = {}
    prefix = code[:pos]
    typedef_pattern = re.compile(
        r"\btypedef\s+struct(?:\s+(?P<tag>[A-Za-z_][A-Za-z0-9_]*))?\s*\{(?P<body>.*?)\}\s*(?P<aliases>[^;]+);",
        flags=re.S,
    )
    for match in typedef_pattern.finditer(prefix):
        fields = parse_char_array_fields(code, match.group("body"), match.end())
        if not fields:
            continue
        tag = match.group("tag")
        if tag:
            type_fields[f"struct {tag}"] = fields
        for alias_part in split_arguments(match.group("aliases")):
            alias = declarator_name(alias_part)
            if alias:
                type_fields[alias] = fields

    struct_pattern = re.compile(
        r"(?<!typedef\s)\bstruct\s+(?P<tag>[A-Za-z_][A-Za-z0-9_]*)\s*\{(?P<body>.*?)\}\s*;",
        flags=re.S,
    )
    for match in struct_pattern.finditer(prefix):
        fields = parse_char_array_fields(code, match.group("body"), match.end())
        if fields:
            type_fields[f"struct {match.group('tag')}"] = fields

    alias_pattern = re.compile(
        r"\btypedef\s+struct\s+(?P<tag>[A-Za-z_][A-Za-z0-9_]*)\s+(?P<aliases>[^;{}]+);",
        flags=re.S,
    )
    for match in alias_pattern.finditer(prefix):
        fields = type_fields.get(f"struct {match.group('tag')}")
        if not fields:
            continue
        for alias_part in split_arguments(match.group("aliases")):
            alias = declarator_name(alias_part)
            if alias:
                type_fields[alias] = fields
    return type_fields


def variable_type_before(code: str, name: str, pos: int) -> str | None:
    type_fields = struct_array_fields_before(code, pos)
    if not type_fields:
        return None
    prefix = code[:pos]
    for type_name in sorted(type_fields, key=len, reverse=True):
        declaration = re.compile(
            rf"(?m)(?:^|[;{{}}\n])\s*(?:const\s+)?{re.escape(type_name)}\s+(?P<decls>[^;]+);"
        )
        for match in declaration.finditer(prefix):
            for part in split_arguments(match.group("decls")):
                var_name = declarator_name(part)
                if var_name == name:
                    return type_name
    return None


def member_expression_parts(expr: str) -> tuple[str, str] | None:
    compact = compact_expr(expr)
    match = re.search(r"(?P<base>.+?)(?:->|\.)(?P<field>[A-Za-z_][A-Za-z0-9_]*)$", compact)
    if not match:
        return None
    base_expr = match.group("base")
    field = match.group("field")
    base_match = re.match(r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)(?:\[.*\])?$", base_expr)
    if not base_match:
        return None
    return base_match.group("name"), field


def typed_member_array_capacity_before(code: str, expr: str, pos: int) -> int | None:
    parts = member_expression_parts(expr)
    if parts is None:
        return None
    base_name, field = parts
    type_name = variable_type_before(code, base_name, pos)
    if type_name is None:
        return None
    return struct_array_fields_before(code, pos).get(type_name, {}).get(field)


def allocation_size_value_before(code: str, args: list[str], call_name: str, pos: int) -> int | None:
    if call_name == "malloc" and args:
        return integer_value_before(code, args[0], pos)
    if call_name == "realloc" and len(args) >= 2:
        return integer_value_before(code, args[1], pos)
    if call_name == "calloc" and len(args) >= 2:
        count = integer_value_before(code, args[0], pos)
        size = integer_value_before(code, args[1], pos)
        if count is not None and size is not None:
            return count * size
    return None


def heap_allocation_capacity_before(code: str, name: str, pos: int) -> int | None:
    best: int | None = None
    prefix = code[:pos]
    for call in iter_function_calls(prefix, {"malloc", "calloc", "realloc", "strdup"}):
        lhs = assignment_lhs_before_call(prefix, call)
        if lhs is None or normalize_lhs(lhs) != name:
            continue
        if call.name == "strdup" and call.args:
            best = source_string_capacity_before(code, call.args[0], call.start)
        else:
            best = allocation_size_value_before(code, call.args, call.name, call.start)
    return best


def expression_capacity_before(code: str, expr: str, pos: int) -> int | None:
    simple = simple_identifier_expr(expr)
    if simple is not None:
        array_capacity = char_array_capacity_before(code, simple, pos)
        if array_capacity is not None:
            return array_capacity
        return heap_allocation_capacity_before(code, simple, pos)
    member_capacity = typed_member_array_capacity_before(code, expr, pos)
    if member_capacity is not None:
        return member_capacity
    return None


def destination_capacity_before(code: str, expr: str, pos: int) -> int | None:
    return expression_capacity_before(code, expr, pos)


def source_string_capacity_before(code: str, expr: str, pos: int) -> int | None:
    literal_len = c_string_literal_length(expr)
    if literal_len is not None:
        return literal_len + 1
    return expression_capacity_before(code, expr, pos)


def bounded_string_copy_status(code: str, dest_expr: str, src_expr: str, pos: int) -> str:
    dest_capacity = destination_capacity_before(code, dest_expr, pos)
    source_capacity = source_string_capacity_before(code, src_expr, pos)
    if dest_capacity is None:
        return "unknown"
    if source_capacity is None:
        return "unknown"
    return "safe" if source_capacity <= dest_capacity else "overflow"


def malloc_size_matches_strlen_plus_one(size_expr: str, src_expr: str) -> bool:
    size = compact_expr(size_expr)
    src = re.escape(compact_expr(src_expr))
    strlen_src = rf"strlen\({src}\)"
    patterns = [
        rf"{strlen_src}\+1",
        rf"1\+{strlen_src}",
        rf"\({strlen_src}\+1\)\*sizeof\(char\)",
        rf"sizeof\(char\)\*\({strlen_src}\+1\)",
        rf"\(1\+{strlen_src}\)\*sizeof\(char\)",
        rf"sizeof\(char\)\*\(1\+{strlen_src}\)",
    ]
    return any(re.fullmatch(pattern, size) for pattern in patterns)


def simple_identifier(expr: str) -> str | None:
    text = compact_expr(expr)
    return text if re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", text) else None


def assignment_to_expr_after(code: str, expr: str, start: int, end: int) -> bool:
    target = re.escape(compact_expr(expr))
    segment = compact_expr(code[start:end])
    return bool(re.search(rf"(?<![A-Za-z0-9_]){target}(?:=(?!=)|[+\-*/%&|^]=|\+\+|--)", segment))


def free_or_realloc_expr_after(code: str, expr: str, start: int, end: int) -> bool:
    target = re.escape(compact_expr(expr))
    segment = compact_expr(code[start:end])
    return bool(
        re.search(rf"(?<![A-Za-z0-9_])free\({target}\)", segment)
        or re.search(rf"(?<![A-Za-z0-9_])realloc\({target},", segment)
    )


def source_value_changed_after(code: str, src_expr: str, start: int, end: int) -> bool:
    name = simple_identifier(src_expr)
    if name is None:
        return False
    escaped = re.escape(name)
    segment = compact_expr(code[start:end])
    if re.search(rf"(?<![A-Za-z0-9_]){escaped}(?:=(?!=)|[+\-*/%&|^]=|\+\+|--)", segment):
        return True
    if re.search(rf"(?<![A-Za-z0-9_])(strcpy|strncpy|strcat|strncat|fgets|gets)\({escaped},", segment):
        return True
    if re.search(rf"scanf\([^;]*&?{escaped}(?:,|\))", segment):
        return True
    return False


def exact_source_string_allocation_before(code: str, dest_expr: str, src_expr: str, pos: int) -> bool:
    """Detect a still-current dst allocation sized exactly for src before strcpy(dst, src)."""
    dest = normalize_lhs(dest_expr)
    prefix = code[:pos]
    for call in iter_function_calls(prefix, {"malloc", "calloc", "realloc", "strdup"}):
        lhs = assignment_lhs_before_call(prefix, call)
        if lhs is None or normalize_lhs(lhs) != dest:
            continue
        if assignment_to_expr_after(code, dest_expr, call.end, pos):
            continue
        if free_or_realloc_expr_after(code, dest_expr, call.end, pos):
            continue
        if source_value_changed_after(code, src_expr, call.end, pos):
            continue
        if call.name == "strdup" and call.args and compact_expr(call.args[0]) == compact_expr(src_expr):
            return True
        if call.name == "malloc" and call.args and malloc_size_matches_strlen_plus_one(call.args[0], src_expr):
            return True
    return False


def compact_statement_slice(code: str, start: int, max_chars: int = 400) -> str:
    return compact_expr(code[start : min(len(code), start + max_chars)])


def strncpy_forces_nul(code: str, dest_expr: str, size_expr: str, call_end: int, dest_capacity: int | None) -> bool:
    after = compact_statement_slice(code, call_end)
    dest = compact_expr(dest_expr)
    size = compact_expr(size_expr)
    index_candidates = {size}
    if dest_capacity is not None:
        index_candidates.add(str(dest_capacity - 1))
    if size.endswith("-1"):
        index_candidates.add(size)
    for index in index_candidates:
        if not index:
            continue
        pattern = rf"{re.escape(dest)}\[{re.escape(index)}\]=(?:'\\0'|0|NULL)"
        if re.search(pattern, after):
            return True
    return False


def format_string_arg_positions(fmt: str) -> list[tuple[dict[str, object], int]]:
    positions: list[tuple[dict[str, object], int]] = []
    arg_index = 0
    for spec in iter_format_specs(fmt):
        conv = str(spec["conv"])
        if spec["suppressed"]:
            continue
        if conv in "*":
            continue
        if conv in {"s", "["}:
            positions.append((spec, arg_index))
        if conv != "%":
            arg_index += 1
    return positions


def literal_format_min_length(fmt: str) -> int | None:
    length = 0
    index = 0
    while index < len(fmt):
        if fmt[index] != "%":
            length += 1
            index += 1
            continue
        if index + 1 < len(fmt) and fmt[index + 1] == "%":
            length += 1
            index += 2
            continue
        return None
    return length


def has_null_check(window: str, target: str) -> bool:
    target = re.escape(target)
    patterns = [
        rf"!\s*{target}\b",
        rf"{target}\s*==\s*NULL",
        rf"NULL\s*==\s*{target}",
        rf"\bif\s*\(\s*{target}\s*(?:\)|&&|\|\|)",
    ]
    return any(re.search(pattern, window) for pattern in patterns)


def assignment_lhs_before_call(code: str, call: FunctionCall) -> str | None:
    statement_start = max(
        code.rfind(";", 0, call.start),
        code.rfind("{", 0, call.start),
        code.rfind("}", 0, call.start),
    )
    prefix = code[statement_start + 1 : call.start]
    match = re.search(
        r"(?P<lhs>[A-Za-z_][A-Za-z0-9_]*(?:\s*(?:\[[^\]]+\]|->\s*[A-Za-z_][A-Za-z0-9_]*|\.\s*[A-Za-z_][A-Za-z0-9_]*))*)\s*=\s*$",
        prefix,
        flags=re.S,
    )
    return re.sub(r"\s+", "", match.group("lhs")) if match else None


def add_finding(
    findings: list[Finding],
    path: Path,
    source: str,
    line: int,
    severity: str,
    confidence: str,
    category: str,
    check_id: str,
    message: str,
) -> None:
    findings.append(
        Finding(
            file=str(path),
            line=line,
            severity=severity,
            confidence=confidence,
            category=category,
            check_id=check_id,
            message=message,
            evidence=line_text(source, line),
        )
    )


def iter_format_specs(fmt: str) -> list[dict[str, object]]:
    specs: list[dict[str, object]] = []
    index = 0
    while index < len(fmt):
        if fmt[index] != "%":
            index += 1
            continue
        start = index
        index += 1
        if index < len(fmt) and fmt[index] == "%":
            index += 1
            continue
        suppressed = False
        if index < len(fmt) and fmt[index] == "*":
            suppressed = True
            index += 1
        while index < len(fmt) and fmt[index] in "-+ #0'":
            index += 1
        width = ""
        while index < len(fmt) and fmt[index].isdigit():
            width += fmt[index]
            index += 1
        if index < len(fmt) and fmt[index] == ".":
            index += 1
            if index < len(fmt) and fmt[index] == "*":
                index += 1
            else:
                while index < len(fmt) and fmt[index].isdigit():
                    index += 1
        for length in ("hh", "ll", "I64", "I32"):
            if fmt.startswith(length, index):
                index += len(length)
                break
        else:
            if index < len(fmt) and fmt[index] in "hljztLqI":
                index += 1
        if index < len(fmt):
            conv = fmt[index]
            if conv == "[":
                closing = fmt.find("]", index + 1)
                index = len(fmt) if closing == -1 else closing
            specs.append(
                {
                    "text": fmt[start : index + 1],
                    "conv": conv,
                    "width": int(width) if width else None,
                    "suppressed": suppressed,
                }
            )
            index += 1
    return specs


def has_input_source(expr: str) -> bool:
    expr = mask_literals_preserve_lines(expr)
    return bool(
        re.search(
            r"\b(argv\s*\[|getenv\s*\(|gets\s*\(|fgets\s*\(|scanf\s*\(|fscanf\s*\(|sscanf\s*\(|"
            r"getchar\s*\(|fgetc\s*\(|fread\s*\(|read\s*\(|recv\s*\()",
            expr,
        )
    )


def contains_identifier(expr: str, name: str) -> bool:
    return bool(re.search(rf"\b{re.escape(name)}\b", mask_literals_preserve_lines(expr)))


def argument_variable(arg: str) -> str | None:
    text = re.sub(r"^\s*[&*]+\s*", "", arg.strip())
    match = re.match(r"([A-Za-z_][A-Za-z0-9_]*)", text)
    return match.group(1) if match else None


def collect_tainted_vars(code: str) -> set[str]:
    tainted: set[str] = set()
    for match in re.finditer(r"\bargv\s*\[\s*[0-9]+\s*\]", code):
        tainted.add(match.group(0))
    for line in code.splitlines():
        for call in iter_function_calls(line, {"gets", "fgets", "read", "recv", "fread"}):
            if call.args:
                var = argument_variable(call.args[0])
                if var:
                    tainted.add(var)
        for call in iter_function_calls(line, {"scanf", "fscanf", "sscanf"}):
            start_index = 1 if call.name == "scanf" else 2
            for arg in call.args[start_index:]:
                var = argument_variable(arg)
                if var:
                    tainted.add(var)
        assign = re.search(r"\b(?P<lhs>[A-Za-z_][A-Za-z0-9_]*)\s*=\s*(?P<rhs>[^;]+)", line)
        if assign:
            lhs = assign.group("lhs")
            rhs = assign.group("rhs")
            if has_input_source(rhs) or any(contains_identifier(rhs, var) for var in tainted):
                tainted.add(lhs)
    return tainted


def parameter_name(param: str) -> str | None:
    text = re.sub(r"=.*$", "", param.strip())
    text = re.sub(r"\[[^\]]*\]", "", text)
    names = re.findall(r"[A-Za-z_][A-Za-z0-9_]*", text)
    if not names or names[-1] in {"void", "char", "int", "long", "short", "double", "float", "FILE"}:
        return None
    return names[-1]


def iter_function_defs(code: str) -> list[FunctionDef]:
    masked = mask_literals_preserve_lines(code)
    pattern = re.compile(
        r"\b(?:static\s+)?(?:inline\s+)?(?:[A-Za-z_][A-Za-z0-9_*\s]+?)\s+"
        r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*\((?P<params>[^;{}()]*)\)\s*\{",
        flags=re.S,
    )
    skip = {"if", "for", "while", "switch"}
    functions: list[FunctionDef] = []
    for match in pattern.finditer(masked):
        name = match.group("name")
        if name in skip:
            continue
        open_brace = masked.find("{", match.start())
        close_brace = find_matching_delimiter(code, open_brace, "{", "}")
        if close_brace is None:
            continue
        params = [param for param in (parameter_name(part) for part in split_arguments(match.group("params"))) if param]
        functions.append(
            FunctionDef(
                name=name,
                params=params,
                body=code[open_brace + 1 : close_brace],
                body_start=open_brace + 1,
                body_end=close_brace,
            )
        )
    return functions


def expression_is_tainted(expr: str, tainted_vars: set[str]) -> bool:
    return has_input_source(expr) or any(contains_identifier(expr, var) for var in tainted_vars)


def scan_dangerous_calls(path: Path, source: str, code: str) -> list[Finding]:
    findings: list[Finding] = []
    tainted = collect_tainted_vars(code)
    for call in iter_function_calls(
        code,
        {
            "gets",
            "strcpy",
            "strcat",
            "sprintf",
            "vsprintf",
            "tmpnam",
            "strncpy",
            "strncat",
            "atoi",
            "atol",
            "atoll",
            "memcpy",
            "memmove",
            "system",
            "popen",
        },
    ):
        if call.name in {"gets", "tmpnam"}:
            add_finding(
                findings,
                path,
                source,
                call.line,
                "high",
                "confirmed",
                "dangerous_function",
                f"dangerous_function:{call.name}",
                f"{call.name} is unsafe in C code",
            )
        elif call.name in {"system", "popen"}:
            arg = call.args[0] if call.args else ""
            confidence = "confirmed" if expression_is_tainted(arg, tainted) else "likely"
            add_finding(
                findings,
                path,
                source,
                call.line,
                "high",
                confidence,
                "command",
                f"dangerous_function:{call.name}",
                f"{call.name} executes a command; command execution is security-sensitive",
            )
        elif call.name == "strcpy" and len(call.args) >= 2:
            status = bounded_string_copy_status(code, call.args[0], call.args[1], call.start)
            if status == "safe" or exact_source_string_allocation_before(code, call.args[0], call.args[1], call.start):
                continue
            severity = "high" if status == "overflow" else "medium"
            confidence = "confirmed" if status == "overflow" else "review"
            add_finding(
                findings,
                path,
                source,
                call.line,
                severity,
                confidence,
                "dangerous_function",
                "dangerous_function:strcpy",
                "strcpy is used without a proven safe source/destination bound",
            )
        elif call.name == "strcat" and len(call.args) >= 2:
            severity = "high" if expression_is_tainted(call.args[1], tainted) else "medium"
            confidence = "confirmed" if severity == "high" else "review"
            add_finding(
                findings,
                path,
                source,
                call.line,
                severity,
                confidence,
                "dangerous_function",
                "dangerous_function:strcat",
                "strcat depends on remaining destination capacity, which is not proven here",
            )
        elif call.name in {"sprintf", "vsprintf"}:
            if len(call.args) >= 2 and call.name == "sprintf":
                dest_capacity = destination_capacity_before(code, call.args[0], call.start)
                fmt = extract_c_string_literal(call.args[1])
                literal_len = literal_format_min_length(fmt) if fmt is not None else None
                if literal_len is not None and dest_capacity is not None and literal_len + 1 <= dest_capacity:
                    continue
                if literal_len is not None and dest_capacity is not None and literal_len + 1 > dest_capacity:
                    severity = "high"
                    confidence = "confirmed"
                else:
                    severity = "medium"
                    confidence = "review"
            else:
                severity = "medium"
                confidence = "review"
            add_finding(
                findings,
                path,
                source,
                call.line,
                severity,
                confidence,
                "dangerous_function",
                f"dangerous_function:{call.name}",
                f"{call.name} writes without an explicit destination size",
            )
        elif call.name == "strncpy" and len(call.args) >= 3:
            dest_capacity = destination_capacity_before(code, call.args[0], call.start)
            size_value = integer_value_before(code, call.args[2], call.start)
            if (
                dest_capacity is not None
                and size_value is not None
                and size_value <= dest_capacity - 1
                and strncpy_forces_nul(code, call.args[0], call.args[2], call.end, dest_capacity)
            ):
                continue
            severity = "medium" if dest_capacity is not None and size_value == dest_capacity else "low"
            add_finding(
                findings,
                path,
                source,
                call.line,
                severity,
                "review",
                "memory",
                "dangerous_function:strncpy",
                "strncpy can leave the destination unterminated unless null termination is proven",
            )
        elif call.name == "strncat":
            add_finding(
                findings,
                path,
                source,
                call.line,
                "low",
                "review",
                "memory",
                "dangerous_function:strncat",
                "strncat requires a remaining-capacity calculation",
            )
        elif call.name in {"memcpy", "memmove"} and len(call.args) >= 3:
            dest_capacity = destination_capacity_before(code, call.args[0], call.start)
            size_value = integer_value_before(code, call.args[2], call.start)
            if dest_capacity is not None and size_value is not None and size_value <= dest_capacity:
                continue
            add_finding(
                findings,
                path,
                source,
                call.line,
                "medium" if expression_is_tainted(",".join(call.args[1:]), tainted) else "low",
                "review",
                "memory",
                f"dangerous_function:{call.name}",
                f"{call.name} requires a destination bound that is not proven here",
            )
        elif call.name in {"atoi", "atol", "atoll"}:
            add_finding(
                findings,
                path,
                source,
                call.line,
                "low",
                "review",
                "integer",
                f"dangerous_function:{call.name}",
                f"{call.name} has no reliable error reporting for invalid numeric input",
            )
    return findings


def scan_scanf_widths(path: Path, source: str, code: str) -> list[Finding]:
    findings: list[Finding] = []
    format_arg_index = {"scanf": 0, "fscanf": 1, "sscanf": 1}
    first_output_arg_index = {"scanf": 1, "fscanf": 2, "sscanf": 2}
    for call in iter_function_calls(code, set(format_arg_index)):
        fmt_index = format_arg_index[call.name]
        if len(call.args) <= fmt_index:
            continue
        fmt = extract_c_string_literal(call.args[fmt_index])
        if fmt is None:
            add_finding(
                findings,
                path,
                source,
                call.line,
                "medium",
                "review",
                "input",
                f"{call.name}_nonliteral_format",
                f"{call.name} uses a non-literal format string; manual review is required",
            )
            continue
        consumed_args = 0
        source_capacity = None
        if call.name == "sscanf" and call.args:
            source_capacity = source_string_capacity_before(code, call.args[0], call.start)
        for spec in iter_format_specs(fmt):
            if spec["suppressed"]:
                continue
            output_arg_index = first_output_arg_index[call.name] + consumed_args
            consumed_args += 1
            if spec["conv"] not in {"s", "["}:
                continue
            dest_expr = call.args[output_arg_index] if len(call.args) > output_arg_index else ""
            dest_capacity = destination_capacity_before(code, dest_expr, call.start)
            width = spec["width"]
            if width is None:
                if source_capacity is not None and dest_capacity is not None and source_capacity <= dest_capacity:
                    continue
                add_finding(
                    findings,
                    path,
                    source,
                    call.line,
                    "high",
                    "confirmed",
                    "input",
                    "scanf_unbounded_string_conversion",
                    f"{call.name} format contains unbounded {spec['text']} conversion",
                )
            elif dest_capacity is not None and int(width) + 1 > dest_capacity:
                add_finding(
                    findings,
                    path,
                    source,
                    call.line,
                    "high",
                    "confirmed",
                    "input",
                    "scanf_width_exceeds_destination",
                    f"{call.name} width {width} can write past destination capacity {dest_capacity}",
                )
            elif dest_capacity is None:
                add_finding(
                    findings,
                    path,
                    source,
                    call.line,
                    "low",
                    "review",
                    "input",
                    "scanf_width_destination_unknown",
                    f"{call.name} uses width {width}, but destination capacity is not proven",
                )
    return findings


def scan_dynamic_format_strings(path: Path, source: str, code: str) -> list[Finding]:
    findings: list[Finding] = []
    tainted = collect_tainted_vars(code)
    format_arg_index = {
        "printf": 0,
        "fprintf": 1,
        "sprintf": 1,
        "snprintf": 2,
        "vprintf": 0,
        "vfprintf": 1,
        "vsprintf": 1,
        "vsnprintf": 2,
    }
    for call in iter_function_calls(code, set(format_arg_index)):
        index = format_arg_index[call.name]
        if len(call.args) <= index:
            continue
        if extract_c_string_literal(call.args[index]) is None:
            severity = "high" if expression_is_tainted(call.args[index], tainted) else "medium"
            confidence = "confirmed" if severity == "high" else "review"
            add_finding(
                findings,
                path,
                source,
                call.line,
                severity,
                confidence,
                "format",
                f"nonliteral_{call.name}_format",
                f"{call.name} uses a dynamic format string",
            )
    return findings


def explicit_bound_exceeds_destination(code: str, dest_expr: str, size_expr: str, pos: int) -> bool:
    capacity = destination_capacity_before(code, dest_expr, pos)
    size = integer_value_before(code, size_expr, pos)
    return capacity is not None and size is not None and size > capacity


def scan_explicit_bounds(path: Path, source: str, code: str) -> list[Finding]:
    findings: list[Finding] = []
    bounded_calls = {
        "fgets": (0, 1, "input"),
        "snprintf": (0, 1, "format"),
        "vsnprintf": (0, 1, "format"),
        "strncpy": (0, 2, "string"),
        "memcpy": (0, 2, "memory"),
        "memmove": (0, 2, "memory"),
    }
    for call in iter_function_calls(code, set(bounded_calls)):
        dest_index, size_index, category = bounded_calls[call.name]
        if len(call.args) <= max(dest_index, size_index):
            continue
        if explicit_bound_exceeds_destination(code, call.args[dest_index], call.args[size_index], call.start):
            add_finding(
                findings,
                path,
                source,
                call.line,
                "high",
                "confirmed",
                category,
                f"explicit_bound_exceeds_destination:{call.name}",
                f"{call.name} is passed a size larger than the destination buffer capacity",
            )
    return findings


def normalize_lhs(expr: str) -> str:
    return re.sub(r"\s+", "", expr)


def scan_allocations(path: Path, source: str, code: str) -> list[Finding]:
    findings: list[Finding] = []
    lines = code.splitlines()
    for call in iter_function_calls(code, {"malloc", "calloc", "realloc", "strdup"}):
        lhs = assignment_lhs_before_call(code, call)
        idx = call.line - 1
        window = "\n".join(lines[idx : idx + 10])
        if lhs:
            direct_realloc = False
            if call.name == "realloc" and call.args:
                first_arg = normalize_lhs(call.args[0])
                if first_arg.startswith(lhs):
                    direct_realloc = True
                    add_finding(
                        findings,
                        path,
                        source,
                        call.line,
                        "medium",
                        "likely",
                        "memory",
                        "realloc_direct_assign",
                        "realloc result is assigned directly to the original pointer",
                    )
            if not direct_realloc and not has_null_check(window, lhs):
                add_finding(
                    findings,
                    path,
                    source,
                    call.line,
                    "medium",
                    "likely",
                    "memory",
                    f"unchecked_allocation:{call.name}",
                    f"{call.name} result does not appear to be checked for NULL nearby",
                )

        if call.name in {"malloc", "calloc", "realloc"} and "*" in ",".join(call.args):
            before = code[max(0, call.start - 200) : call.start]
            if not re.search(r"(SIZE_MAX|UINT_MAX|overflow|if\s*\()", before):
                add_finding(
                    findings,
                    path,
                    source,
                    call.line,
                    "medium",
                    "review",
                    "integer",
                    "allocation_size_without_overflow_guard",
                    "allocation size multiplication has no visible overflow guard",
                )
    return findings


def argv_index(expr: str) -> int | None:
    match = re.search(r"\bargv\s*\[\s*(?P<index>[0-9]+)\s*\]", expr)
    return int(match.group("index")) if match else None


def argc_guard_exists_before(code: str, index: int, pos: int) -> bool:
    if index <= 0:
        return True
    prefix = code[:pos]
    required = index + 1
    patterns = [
        rf"\bif\s*\(\s*argc\s*<\s*{required}\s*\)\s*\{{?[^{{}}]*\b(?:return|exit)\b",
        rf"\bif\s*\(\s*{required}\s*>\s*argc\s*\)\s*\{{?[^{{}}]*\b(?:return|exit)\b",
        rf"\bif\s*\(\s*argc\s*<=\s*{index}\s*\)\s*\{{?[^{{}}]*\b(?:return|exit)\b",
        rf"\bif\s*\(\s*{index}\s*>=\s*argc\s*\)\s*\{{?[^{{}}]*\b(?:return|exit)\b",
        rf"\bargc\s*>=\s*{required}\b",
        rf"\b{required}\s*<=\s*argc\b",
        rf"\bargc\s*>\s*{index}\b",
        rf"\b{index}\s*<\s*argc\b",
    ]
    return any(re.search(pattern, prefix, flags=re.S) for pattern in patterns)


def scan_files_and_args(path: Path, source: str, code: str) -> list[Finding]:
    findings: list[Finding] = []
    lines = code.splitlines()
    tainted = collect_tainted_vars(code)
    fopen_calls = iter_function_calls(code, {"fopen"})
    if len(fopen_calls) > len(iter_function_calls(code, {"fclose"})):
        add_finding(
            findings,
            path,
            source,
            fopen_calls[0].line if fopen_calls else 1,
            "medium",
            "review",
            "resource",
            "fopen_without_matching_fclose",
            "more fopen calls than fclose calls",
        )
    for call in fopen_calls:
        lhs = assignment_lhs_before_call(code, call)
        if lhs:
            idx = call.line - 1
            window = "\n".join(lines[idx : idx + 8])
            if not has_null_check(window, lhs):
                add_finding(
                    findings,
                    path,
                    source,
                    call.line,
                    "medium",
                    "likely",
                    "file",
                    "unchecked_fopen",
                    "fopen result does not appear to be checked for NULL nearby",
                )
        path_expr = call.args[0] if call.args else ""
        if expression_is_tainted(path_expr, tainted) or argv_index(path_expr) is not None:
            add_finding(
                findings,
                path,
                source,
                call.line,
                "medium",
                "review",
                "file",
                "external_path_fopen",
                "fopen receives an externally controlled path; review intended file access policy",
            )

    argv_uses = list(re.finditer(r"\bargv\s*\[\s*(?P<index>[0-9]+)\s*\]", code))
    if argv_uses and not re.search(r"\bargc\b", code):
        add_finding(
            findings,
            path,
            source,
            code[: argv_uses[0].start()].count("\n") + 1,
            "high",
            "confirmed",
            "input",
            "argv_used_without_argc",
            "argv index is used but argc does not appear in the source",
        )
    for match in argv_uses:
        index = int(match.group("index"))
        if not argc_guard_exists_before(code, index, match.start()):
            add_finding(
                findings,
                path,
                source,
                code[: match.start()].count("\n") + 1,
                "medium",
                "likely",
                "input",
                "argv_without_sufficient_argc_check",
                f"argv[{index}] is used without a clear prior argc guard",
            )
    return findings


def scan_fragile_patterns(path: Path, source: str, code: str) -> list[Finding]:
    findings: list[Finding] = []
    for line_no, line in enumerate(code.splitlines(), start=1):
        if re.search(r"\bwhile\s*\(\s*!\s*feof\s*\(", line):
            add_finding(
                findings,
                path,
                source,
                line_no,
                "medium",
                "review",
                "input",
                "while_not_feof",
                "while (!feof(...)) is a fragile file-reading pattern",
            )
    return findings


def scan_tainted_sinks(path: Path, source: str, code: str) -> list[Finding]:
    findings: list[Finding] = []
    tainted = collect_tainted_vars(code)
    sinks = {
        "strcpy": ("high", "string", [1]),
        "strcat": ("high", "string", [1]),
        "memcpy": ("medium", "memory", [1, 2]),
        "memmove": ("medium", "memory", [1, 2]),
        "system": ("high", "command", [0]),
        "popen": ("high", "command", [0]),
    }

    def maybe_add(call: FunctionCall, line: int, local_tainted: set[str]) -> None:
        severity, category, indexes = sinks[call.name]
        for index in indexes:
            if len(call.args) <= index or not expression_is_tainted(call.args[index], local_tainted):
                continue
            if call.name == "strcpy" and len(call.args) >= 2:
                status = bounded_string_copy_status(code, call.args[0], call.args[1], call.start)
                if status == "safe" or exact_source_string_allocation_before(code, call.args[0], call.args[1], call.start):
                    return
            if call.name in {"memcpy", "memmove"} and len(call.args) >= 3:
                dest_capacity = destination_capacity_before(code, call.args[0], call.start)
                size_value = integer_value_before(code, call.args[2], call.start)
                if dest_capacity is not None and size_value is not None and size_value <= dest_capacity:
                    return
            add_finding(
                findings,
                path,
                source,
                line,
                severity,
                "confirmed",
                category,
                f"tainted_sink:{call.name}",
                f"externally controlled input appears to reach {call.name} argument {index + 1}",
            )
            return

    for call in iter_function_calls(code, set(sinks)):
        maybe_add(call, call.line, tainted)

    for function in iter_function_defs(code):
        function_calls = [call for call in iter_function_calls(code, {function.name}) if call.start < function.body_start or call.start > function.body_end]
        for outer_call in function_calls:
            local_tainted = set(tainted)
            for index, param in enumerate(function.params):
                if len(outer_call.args) > index and expression_is_tainted(outer_call.args[index], tainted):
                    local_tainted.add(param)
            if local_tainted == tainted:
                continue
            for inner_call in iter_function_calls(function.body, set(sinks)):
                global_start = function.body_start + inner_call.start
                global_line = code[:global_start].count("\n") + 1
                global_call = FunctionCall(
                    name=inner_call.name,
                    args=inner_call.args,
                    line=global_line,
                    text=inner_call.text,
                    start=global_start,
                    end=function.body_start + inner_call.end,
                )
                maybe_add(global_call, global_line, local_tainted)
    return findings


def custom_scan(path: Path) -> tuple[list[Finding], dict[str, int]]:
    source = read_text(path)
    code = strip_comments_preserve_lines(source)
    findings: list[Finding] = []
    findings.extend(scan_dangerous_calls(path, source, code))
    findings.extend(scan_scanf_widths(path, source, code))
    findings.extend(scan_dynamic_format_strings(path, source, code))
    findings.extend(scan_explicit_bounds(path, source, code))
    findings.extend(scan_allocations(path, source, code))
    findings.extend(scan_files_and_args(path, source, code))
    findings.extend(scan_fragile_patterns(path, source, code))
    findings.extend(scan_tainted_sinks(path, source, code))
    findings = deduplicate_findings(findings)
    stats = {
        "line_count": len(source.splitlines()),
        "byte_count": len(source.encode("utf-8", errors="replace")),
        "finding_count": len(findings),
    }
    return findings, stats


def deduplicate_findings(findings: list[Finding]) -> list[Finding]:
    severity_rank = {"info": 0, "low": 1, "medium": 2, "high": 3}
    best: dict[tuple[str, int, str, str, str], Finding] = {}
    order: list[tuple[str, int, str, str, str]] = []
    for finding in findings:
        key = (finding.file, finding.line, finding.check_id, finding.message, finding.evidence)
        current = best.get(key)
        if current is None:
            best[key] = finding
            order.append(key)
        elif severity_rank.get(finding.severity, 0) > severity_rank.get(current.severity, 0):
            best[key] = finding
    return [best[key] for key in order]


def severity_counts(findings: list[Finding]) -> dict[str, int]:
    counts = {"high": 0, "medium": 0, "low": 0, "info": 0}
    for finding in findings:
        counts[finding.severity] = counts.get(finding.severity, 0) + 1
    return counts


def security_score(findings: list[Finding]) -> int:
    return sum(SEVERITY_WEIGHT.get(finding.severity, 0) for finding in deduplicate_findings(findings))


MEMORY_CHECK_IDS = {
    "allocation_size_without_overflow_guard",
    "argv_used_without_argc",
    "argv_without_sufficient_argc_check",
    "realloc_direct_assign",
    "scanf_unbounded_string_conversion",
    "scanf_width_destination_unknown",
    "scanf_width_exceeds_destination",
}

MEMORY_CHECK_PREFIXES = (
    "dangerous_function:gets",
    "dangerous_function:memcpy",
    "dangerous_function:memmove",
    "dangerous_function:sprintf",
    "dangerous_function:strcat",
    "dangerous_function:strcpy",
    "dangerous_function:strncat",
    "dangerous_function:strncpy",
    "dangerous_function:vsprintf",
    "explicit_bound_exceeds_destination:",
    "tainted_sink:memcpy",
    "tainted_sink:memmove",
    "tainted_sink:strcat",
    "tainted_sink:strcpy",
    "unchecked_allocation:",
)


def is_memory_safety_finding(finding: Finding) -> bool:
    check_id = finding.check_id
    if check_id in MEMORY_CHECK_IDS:
        return True
    if any(check_id.startswith(prefix) for prefix in MEMORY_CHECK_PREFIXES):
        return True
    if finding.category in {"memory", "string"} and check_id != "dangerous_function:atoi":
        return True
    return False


def finding_domain(finding: Finding) -> str:
    return "memory" if is_memory_safety_finding(finding) else "auxiliary"


def finding_subdomain(finding: Finding) -> str:
    check_id = finding.check_id
    if is_memory_safety_finding(finding):
        if check_id.startswith("unchecked_allocation:") or check_id == "realloc_direct_assign":
            return "memory_heap"
        if check_id == "allocation_size_without_overflow_guard":
            return "memory_integer"
        if check_id.startswith("dangerous_function:") or check_id.startswith("tainted_sink:"):
            if any(name in check_id for name in ("str", "sprintf", "gets", "scanf")):
                return "memory_string"
            return "memory_copy"
        if check_id.startswith("scanf_"):
            return "memory_string"
        if check_id.startswith("argv_"):
            return "memory_bounds"
        if check_id.startswith("explicit_bound_exceeds_destination:"):
            return "memory_bounds"
        return "memory_other"
    if finding.category in {"file", "resource"}:
        return "auxiliary_file_resource"
    if finding.category == "command":
        return "auxiliary_command"
    if finding.category == "format":
        return "auxiliary_format"
    if finding.category == "integer":
        return "auxiliary_integer"
    if finding.category == "input":
        return "auxiliary_input"
    return "auxiliary_other"


def filter_findings_by_domain(findings: list[Finding], domain: str) -> list[Finding]:
    return [finding for finding in findings if finding_domain(finding) == domain]


def domain_score(findings: list[Finding], domain: str) -> int:
    return security_score(filter_findings_by_domain(findings, domain))


def score_findings(findings: list[Finding]) -> int:
    return security_score(findings)


def safe_name(path: Path) -> str:
    return re.sub(r"[^A-Za-z0-9_.-]+", "_", path.stem) or "program"


def run_command(command: list[str], timeout: float, stdin_text: str | None = None) -> tuple[int | str, str, bool]:
    try:
        completed = subprocess.run(
            command,
            input=stdin_text,
            text=True,
            capture_output=True,
            timeout=timeout,
        )
        return completed.returncode, (completed.stdout or "") + (completed.stderr or ""), False
    except subprocess.TimeoutExpired as exc:
        output = ""
        if exc.stdout:
            output += exc.stdout if isinstance(exc.stdout, str) else exc.stdout.decode(errors="replace")
        if exc.stderr:
            output += exc.stderr if isinstance(exc.stderr, str) else exc.stderr.decode(errors="replace")
        return "timeout", output, True
    except OSError as exc:
        return "os_error", str(exc), False


def count_words(output: str, *words: str) -> int:
    return sum(len(re.findall(rf"\b{re.escape(word)}\b", output, flags=re.I)) for word in words)


AUX_STATIC_FIELDS = [
    "file",
    "tool",
    "attempted",
    "status",
    "success",
    "returncode",
    "finding_count",
    "line",
    "severity",
    "check_id",
    "message",
    "evidence",
    "log_path",
]


def tool_status(returncode: int | str, timed_out: bool) -> str:
    if timed_out:
        return "timeout"
    if returncode == "not_found":
        return "not_found"
    if returncode == "skipped":
        return "skipped"
    if returncode == "os_error":
        return "os_error"
    if returncode == 0:
        return "success"
    return "failed"


def parse_int(value: str) -> int | str:
    try:
        return int(value)
    except ValueError:
        return ""


def cppcheck_std(std: str) -> str:
    normalized = std.lower()
    if "23" in normalized:
        return "c23"
    if "17" in normalized:
        return "c17"
    if "11" in normalized:
        return "c11"
    if "99" in normalized:
        return "c99"
    if "89" in normalized or "90" in normalized:
        return "c89"
    return "c11"


def auxiliary_status_row(
    path: Path,
    tool: str,
    attempted: bool,
    status: str,
    success: object,
    returncode: object,
    log_path: Path | str,
) -> dict[str, object]:
    return {
        "file": str(path),
        "tool": tool,
        "attempted": attempted,
        "status": status,
        "success": success,
        "returncode": returncode,
        "finding_count": 0,
        "line": "",
        "severity": "",
        "check_id": "",
        "message": "",
        "evidence": "",
        "log_path": str(log_path),
    }


def skipped_auxiliary_rows(path: Path) -> tuple[list[dict[str, object]], list[dict[str, object]]]:
    return (
        [auxiliary_status_row(path, "cppcheck", False, "skipped", "", "skipped", "")],
        [auxiliary_status_row(path, "clang_analyzer", False, "skipped", "", "skipped", "")],
    )


def run_cppcheck(path: Path, log_dir: Path, std: str, timeout: float) -> list[dict[str, object]]:
    tool = "cppcheck"
    executable = shutil.which(tool)
    log_path = log_dir / f"{safe_name(path)}_cppcheck.log"
    if executable is None:
        log_path.write_text("cppcheck not found\n", encoding="utf-8", errors="replace")
        return [auxiliary_status_row(path, tool, False, "not_found", "", "not_found", log_path)]

    template = "{file}|{line}|{severity}|{id}|{message}"
    command = [
        executable,
        "--enable=warning,style,performance,portability,information",
        "--inconclusive",
        "--force",
        "--quiet",
        f"--std={cppcheck_std(std)}",
        f"--template={template}",
        str(path),
    ]
    returncode, output, timed_out = run_command(command, timeout)
    log_path.write_text(output, encoding="utf-8", errors="replace")
    status = tool_status(returncode, timed_out)

    parsed: list[dict[str, object]] = []
    for raw_line in output.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        parts = line.split("|", 4)
        if len(parts) != 5:
            continue
        file_name, line_no, severity, check_id, message = parts
        parsed.append(
            {
                "file": file_name or str(path),
                "tool": tool,
                "attempted": True,
                "status": status,
                "success": returncode == 0 and not timed_out,
                "returncode": returncode,
                "finding_count": 0,
                "line": parse_int(line_no),
                "severity": severity,
                "check_id": check_id,
                "message": message,
                "evidence": line,
                "log_path": str(log_path),
            }
        )

    finding_count = len(parsed)
    if not parsed:
        return [auxiliary_status_row(path, tool, True, status, returncode == 0 and not timed_out, returncode, log_path)]
    for row in parsed:
        row["finding_count"] = finding_count
    return parsed


def run_clang_analyzer(path: Path, log_dir: Path, std: str, timeout: float) -> list[dict[str, object]]:
    tool = "clang_analyzer"
    executable = shutil.which("clang")
    log_path = log_dir / f"{safe_name(path)}_clang_analyzer.log"
    if executable is None:
        log_path.write_text("clang not found\n", encoding="utf-8", errors="replace")
        return [auxiliary_status_row(path, tool, False, "not_found", "", "not_found", log_path)]

    command = [
        executable,
        "--analyze",
        "-Xanalyzer",
        "-analyzer-output=text",
        "-fno-color-diagnostics",
        f"-std={std}",
        "-Wall",
        "-Wextra",
        str(path),
    ]
    returncode, output, timed_out = run_command(command, timeout)
    log_path.write_text(output, encoding="utf-8", errors="replace")
    status = tool_status(returncode, timed_out)

    diagnostic = re.compile(
        r"^(?P<file>.*?):(?P<line>\d+):(?:(?P<column>\d+):)?\s*"
        r"(?P<severity>warning|error):\s*(?P<message>.*?)(?:\s+\[(?P<check_id>[^\]]+)\])?$"
    )
    parsed: list[dict[str, object]] = []
    for raw_line in output.splitlines():
        line = raw_line.strip()
        match = diagnostic.match(line)
        if not match:
            continue
        check_id = match.group("check_id") or f"clang_analyzer_{match.group('severity')}"
        parsed.append(
            {
                "file": match.group("file") or str(path),
                "tool": tool,
                "attempted": True,
                "status": status,
                "success": returncode == 0 and not timed_out,
                "returncode": returncode,
                "finding_count": 0,
                "line": parse_int(match.group("line")),
                "severity": match.group("severity"),
                "check_id": check_id,
                "message": match.group("message"),
                "evidence": line,
                "log_path": str(log_path),
            }
        )

    finding_count = len(parsed)
    if not parsed:
        return [auxiliary_status_row(path, tool, True, status, returncode == 0 and not timed_out, returncode, log_path)]
    for row in parsed:
        row["finding_count"] = finding_count
    return parsed


def compile_with_tool(
    tool: str,
    path: Path,
    build_dir: Path,
    log_dir: Path,
    std: str,
    timeout: float,
    mode: str,
) -> tuple[dict[str, object], Path | None]:
    executable = shutil.which(tool)
    log_path = log_dir / f"{safe_name(path)}_{tool}_{mode}.log"
    row: dict[str, object] = {
        "file": str(path),
        "tool": tool,
        "mode": mode,
        "attempted": executable is not None,
        "success": False if executable is not None else "",
        "returncode": "not_found" if executable is None else "",
        "warning_count": 0,
        "error_count": 0,
        "log_path": str(log_path),
    }
    if executable is None:
        log_path.write_text(f"{tool} not found\n", encoding="utf-8", errors="replace")
        return row, None

    suffix = ".exe" if os.name == "nt" else ""
    binary = build_dir / f"{safe_name(path)}_{tool}_{mode}{suffix}"
    command = [executable, f"-std={std}", "-Wall", "-Wextra", "-O0", str(path), "-o", str(binary)]
    if mode == "asan":
        command[1:1] = ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    returncode, output, timed_out = run_command(command, timeout)
    log_path.write_text(output, encoding="utf-8", errors="replace")
    row.update(
        {
            "success": returncode == 0 and not timed_out,
            "returncode": returncode,
            "warning_count": count_words(output, "warning"),
            "error_count": count_words(output, "error"),
        }
    )
    return row, binary if row["success"] else None


def skipped_compile_row(tool: str, path: Path, mode: str) -> dict[str, object]:
    return {
        "file": str(path),
        "tool": tool,
        "mode": mode,
        "attempted": False,
        "success": "",
        "returncode": "skipped",
        "warning_count": 0,
        "error_count": 0,
        "log_path": "",
    }


def compile_status(row: dict[str, object]) -> str:
    if not row.get("attempted"):
        return "skipped" if row.get("returncode") == "skipped" else "not_found"
    if row.get("success") is True:
        return "success"
    if row.get("returncode") == "timeout":
        return "timeout"
    return "failed"


def compile_success_value(row: dict[str, object]) -> object:
    return row.get("success") if row.get("attempted") else ""


def is_crash(returncode: int | str, output: str) -> bool:
    if returncode == "timeout":
        return False
    if isinstance(returncode, int) and returncode < 0:
        return True
    lower = output.lower()
    crash_words = [
        "addresssanitizer",
        "undefinedbehavior",
        "runtime error:",
        "segmentation fault",
        "access violation",
        "stack-buffer-overflow",
        "heap-buffer-overflow",
    ]
    return any(word in lower for word in crash_words)


def run_smoke_tests(
    binary: Path | None,
    path: Path,
    log_dir: Path,
    timeout: float,
    runtime_mode: str,
) -> list[dict[str, object]]:
    rows: list[dict[str, object]] = []
    if binary is None:
        for test_name, _, _ in SMOKE_TESTS:
            rows.append(
                {
                    "file": str(path),
                    "runtime_mode": runtime_mode,
                    "test_name": test_name,
                    "attempted": False,
                    "timed_out": False,
                    "crashed": False,
                    "returncode": "not_compiled",
                    "log_path": "",
                }
            )
        return rows
    for test_name, args, stdin_text in SMOKE_TESTS:
        log_path = log_dir / f"{safe_name(path)}_{runtime_mode}_smoke_{test_name}.log"
        command = [str(binary), *args]
        started = time.time()
        returncode, output, timed_out = run_command(command, timeout, stdin_text)
        elapsed = time.time() - started
        crashed = is_crash(returncode, output)
        log_path.write_text(
            f"command={' '.join(command)}\nelapsed_seconds={elapsed:.3f}\nreturncode={returncode}\n"
            f"timed_out={timed_out}\n--- output ---\n{output}",
            encoding="utf-8",
            errors="replace",
        )
        rows.append(
            {
                "file": str(path),
                "runtime_mode": runtime_mode,
                "test_name": test_name,
                "attempted": True,
                "timed_out": timed_out,
                "crashed": crashed,
                "returncode": returncode,
                "log_path": str(log_path),
            }
        )
    return rows


def runtime_crash_score(rows: list[dict[str, object]]) -> int:
    crashes = sum(1 for row in rows if row.get("crashed") is True)
    return crashes * 15


def runtime_timeout_score(rows: list[dict[str, object]]) -> int:
    timeouts = sum(1 for row in rows if row.get("timed_out") is True)
    return timeouts * 5


def runtime_score(rows: list[dict[str, object]]) -> int:
    return runtime_crash_score(rows)


def write_csv(path: Path, rows: list[dict[str, object]], fields: list[str]) -> None:
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def main() -> int:
    args = parse_args()
    if args.static_only:
        args.with_compile = False
        args.with_smoke = False
        args.skip_smoke = True
    else:
        args.with_compile = True
        args.with_smoke = not args.skip_smoke
    src = args.src.resolve()
    if not src.exists() or not src.is_dir():
        print(f"Source directory not found: {src}", file=sys.stderr)
        return 2
    c_files = find_c_files(src, args.recursive)
    args.out.mkdir(parents=True, exist_ok=True)
    log_dir = args.out / "logs"
    build_dir = args.out / "build"
    log_dir.mkdir(parents=True, exist_ok=True)
    build_dir.mkdir(parents=True, exist_ok=True)
    if args.static_only:
        print("Static-only mode: compile checks and smoke tests are skipped.")
    elif args.skip_smoke:
        print("Full scan mode: static checks and compile checks are enabled; smoke tests are skipped.")
    else:
        print("Full scan mode: static checks, compile checks, and smoke tests are enabled.")
    if args.skip_aux_static:
        print("Auxiliary static tools are skipped.")
    else:
        print("Auxiliary static tools are enabled: Cppcheck and Clang Static Analyzer outputs will be written if available.")

    all_findings: list[Finding] = []
    summary_rows: list[dict[str, object]] = []
    compile_rows: list[dict[str, object]] = []
    runtime_rows: list[dict[str, object]] = []
    cppcheck_rows: list[dict[str, object]] = []
    clang_analyzer_rows: list[dict[str, object]] = []

    with tempfile.TemporaryDirectory(prefix="security_scan_fair_"):
        for index, path in enumerate(c_files, start=1):
            print(f"[{index}/{len(c_files)}] {path}")
            findings, stats = custom_scan(path)
            all_findings.extend(findings)
            sev = severity_counts(findings)
            memory_findings = filter_findings_by_domain(findings, "memory")
            auxiliary_findings = filter_findings_by_domain(findings, "auxiliary")
            memory_sev = severity_counts(memory_findings)
            auxiliary_sev = severity_counts(auxiliary_findings)
            sec_score = security_score(findings)
            memory_score = security_score(memory_findings)
            auxiliary_score = security_score(auxiliary_findings)

            if args.skip_aux_static:
                skipped_cppcheck, skipped_clang_analyzer = skipped_auxiliary_rows(path)
                cppcheck_rows.extend(skipped_cppcheck)
                clang_analyzer_rows.extend(skipped_clang_analyzer)
            else:
                cppcheck_rows.extend(run_cppcheck(path, log_dir, args.std, args.timeout))
                clang_analyzer_rows.extend(run_clang_analyzer(path, log_dir, args.std, args.timeout))

            gcc_binary: Path | None = None
            clang_binary: Path | None = None
            asan_binary: Path | None = None
            if args.with_compile:
                gcc_row, gcc_binary = compile_with_tool("gcc", path, build_dir, log_dir, args.std, args.timeout, "normal")
                clang_row, clang_binary = compile_with_tool(
                    "clang", path, build_dir, log_dir, args.std, args.timeout, "normal"
                )
                asan_row, asan_binary = compile_with_tool("gcc", path, build_dir, log_dir, args.std, args.timeout, "asan")
            else:
                gcc_row = skipped_compile_row("gcc", path, "normal")
                clang_row = skipped_compile_row("clang", path, "normal")
                asan_row = skipped_compile_row("gcc", path, "asan")
            compile_rows.extend([gcc_row, clang_row, asan_row])

            asan_runtime: list[dict[str, object]] = []
            normal_runtime: list[dict[str, object]] = []
            if args.with_smoke and not args.skip_smoke:
                asan_runtime = run_smoke_tests(asan_binary, path, log_dir, args.timeout, "asan")
                runtime_rows.extend(asan_runtime)
                normal_binary = gcc_binary or clang_binary
                normal_runtime = run_smoke_tests(normal_binary, path, log_dir, args.timeout, "normal")
                runtime_rows.extend(normal_runtime)

            compile_fail = (gcc_row["attempted"] or clang_row["attempted"]) and not (
                gcc_row["success"] or clang_row["success"]
            )
            compile_penalty = 20 if compile_fail else 0
            memory_runtime_score = runtime_score(asan_runtime)
            normal_runtime_score = runtime_score(normal_runtime)
            asan_runtime_timeout_score = runtime_timeout_score(asan_runtime)
            normal_runtime_timeout_score = runtime_timeout_score(normal_runtime)
            total_score = memory_score + memory_runtime_score + compile_penalty
            summary_rows.append(
                {
                    "score_scope": "memory_only",
                    "file": str(path),
                    "line_count": stats["line_count"],
                    "byte_count": stats["byte_count"],
                    "custom_total": len(findings),
                    "custom_high": sev["high"],
                    "custom_medium": sev["medium"],
                    "custom_low": sev["low"],
                    "custom_info": sev["info"],
                    "memory_total": len(memory_findings),
                    "memory_high": memory_sev["high"],
                    "memory_medium": memory_sev["medium"],
                    "memory_low": memory_sev["low"],
                    "memory_info": memory_sev["info"],
                    "auxiliary_total": len(auxiliary_findings),
                    "auxiliary_high": auxiliary_sev["high"],
                    "auxiliary_medium": auxiliary_sev["medium"],
                    "auxiliary_low": auxiliary_sev["low"],
                    "auxiliary_info": auxiliary_sev["info"],
                    "gcc_compile_attempted": gcc_row["attempted"],
                    "gcc_compile_status": compile_status(gcc_row),
                    "gcc_compile_success": compile_success_value(gcc_row),
                    "clang_compile_attempted": clang_row["attempted"],
                    "clang_compile_status": compile_status(clang_row),
                    "clang_compile_success": compile_success_value(clang_row),
                    "asan_compile_attempted": asan_row["attempted"],
                    "asan_compile_status": compile_status(asan_row),
                    "asan_compile_success": compile_success_value(asan_row),
                    "asan_runtime_attempted": any(row.get("attempted") is True for row in asan_runtime),
                    "asan_runtime_crashes": sum(1 for row in asan_runtime if row.get("crashed") is True),
                    "asan_runtime_timeouts": sum(1 for row in asan_runtime if row.get("timed_out") is True),
                    "normal_runtime_attempted": any(row.get("attempted") is True for row in normal_runtime),
                    "normal_runtime_crashes": sum(1 for row in normal_runtime if row.get("crashed") is True),
                    "normal_runtime_timeouts": sum(1 for row in normal_runtime if row.get("timed_out") is True),
                    "runtime_crashes": sum(1 for row in asan_runtime if row.get("crashed") is True),
                    "runtime_timeouts": sum(1 for row in asan_runtime if row.get("timed_out") is True),
                    "security_score": memory_score,
                    "all_static_security_score": sec_score,
                    "memory_safety_score": memory_score,
                    "auxiliary_security_score": auxiliary_score,
                    "memory_runtime_score": memory_runtime_score,
                    "normal_runtime_score": normal_runtime_score,
                    "asan_runtime_timeout_score": asan_runtime_timeout_score,
                    "normal_runtime_timeout_score": normal_runtime_timeout_score,
                    "runtime_score": memory_runtime_score,
                    "compile_penalty": compile_penalty,
                    "memory_total_score": total_score,
                    "risk_score": total_score,
                    "total_score": total_score,
                }
            )

    write_csv(
        args.out / "issues.csv",
        [
            {
                "file": finding.file,
                "line": finding.line,
                "severity": finding.severity,
                "confidence": finding.confidence,
                "domain": finding_domain(finding),
                "subdomain": finding_subdomain(finding),
                "category": finding.category,
                "check_id": finding.check_id,
                "message": finding.message,
                "evidence": finding.evidence,
            }
            for finding in all_findings
        ],
        ["file", "line", "severity", "confidence", "domain", "subdomain", "category", "check_id", "message", "evidence"],
    )
    write_csv(
        args.out / "summary.csv",
        summary_rows,
        [
            "score_scope",
            "file",
            "line_count",
            "byte_count",
            "custom_total",
            "custom_high",
            "custom_medium",
            "custom_low",
            "custom_info",
            "memory_total",
            "memory_high",
            "memory_medium",
            "memory_low",
            "memory_info",
            "auxiliary_total",
            "auxiliary_high",
            "auxiliary_medium",
            "auxiliary_low",
            "auxiliary_info",
            "gcc_compile_attempted",
            "gcc_compile_status",
            "gcc_compile_success",
            "clang_compile_attempted",
            "clang_compile_status",
            "clang_compile_success",
            "asan_compile_attempted",
            "asan_compile_status",
            "asan_compile_success",
            "asan_runtime_attempted",
            "asan_runtime_crashes",
            "asan_runtime_timeouts",
            "normal_runtime_attempted",
            "normal_runtime_crashes",
            "normal_runtime_timeouts",
            "runtime_crashes",
            "runtime_timeouts",
            "security_score",
            "all_static_security_score",
            "memory_safety_score",
            "auxiliary_security_score",
            "memory_runtime_score",
            "normal_runtime_score",
            "asan_runtime_timeout_score",
            "normal_runtime_timeout_score",
            "runtime_score",
            "compile_penalty",
            "memory_total_score",
            "risk_score",
            "total_score",
        ],
    )
    write_csv(
        args.out / "compile.csv",
        compile_rows,
        ["file", "tool", "mode", "attempted", "success", "returncode", "warning_count", "error_count", "log_path"],
    )
    write_csv(
        args.out / "runtime.csv",
        runtime_rows,
        ["file", "runtime_mode", "test_name", "attempted", "timed_out", "crashed", "returncode", "log_path"],
    )
    write_csv(args.out / "cppcheck.csv", cppcheck_rows, AUX_STATIC_FIELDS)
    write_csv(args.out / "clang_analyzer.csv", clang_analyzer_rows, AUX_STATIC_FIELDS)
    if not args.keep_build and build_dir.exists():
        shutil.rmtree(build_dir, ignore_errors=True)
    print(f"Done. files={len(c_files)}, findings={len(all_findings)}, out={args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
