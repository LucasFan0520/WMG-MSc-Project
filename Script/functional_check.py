#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import dataclasses
import hashlib
import os
import re
import shutil
import subprocess
import sys
import tempfile
import textwrap
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple


TASK_ID_RE = re.compile(r"^F(\d{3})$", re.IGNORECASE)


@dataclasses.dataclass(frozen=True)
class FunctionalCase:
    task_id: str
    name: str
    stdin: str
    contains: Tuple[str, ...] = ()
    ordered: Tuple[str, ...] = ()
    absent: Tuple[str, ...] = ()
    regex: Tuple[str, ...] = ()
    hard_contains: Tuple[str, ...] = ()
    hard_ordered: Tuple[str, ...] = ()
    hard_absent: Tuple[str, ...] = ()
    hard_regex: Tuple[str, ...] = ()
    files: Dict[str, str] = dataclasses.field(default_factory=dict)
    argv: Tuple[str, ...] = ()
    notes: str = ""


def case(
    task_id: str,
    name: str,
    stdin: str,
    *,
    contains: Sequence[str] = (),
    ordered: Sequence[str] = (),
    absent: Sequence[str] = (),
    regex: Sequence[str] = (),
    hard_contains: Optional[Sequence[str]] = None,
    hard_ordered: Optional[Sequence[str]] = None,
    hard_absent: Optional[Sequence[str]] = None,
    hard_regex: Optional[Sequence[str]] = None,
    files: Optional[Dict[str, str]] = None,
    argv: Sequence[str] = (),
    notes: str = "",
) -> FunctionalCase:
    return FunctionalCase(
        task_id=task_id,
        name=name,
        stdin=textwrap.dedent(stdin).strip() + "\n",
        contains=tuple(contains),
        ordered=tuple(ordered),
        absent=tuple(absent),
        regex=tuple(regex),
        hard_contains=tuple(contains if hard_contains is None else hard_contains),
        hard_ordered=tuple(() if hard_ordered is None else hard_ordered),
        hard_absent=tuple(absent if hard_absent is None else hard_absent),
        hard_regex=tuple(regex if hard_regex is None else hard_regex),
        files=dict(files or {}),
        argv=tuple(argv),
        notes=notes,
    )


def build_cases() -> Dict[str, FunctionalCase]:
    cases = [
        case(
            "F001",
            "paragraph notebook add/append/delete/list",
            """
            NEW alpha first paragraph
            APPEND alpha plus more
            NEW beta second paragraph
            SHOW alpha
            REPLACE beta replacement text
            DELETE alpha
            SHOW alpha
            LIST
            END
            """,
            contains=("first paragraph", "plus more", "NOT_FOUND", "beta"),
            ordered=("first paragraph", "plus more", "NOT_FOUND", "beta"),
        ),
        case(
            "F002",
            "tag collector concat/remove/report",
            """
            ADD t1 first message
            ADD t2 second message
            CONCAT t1 t2 both
            FIND both
            REMOVE t1
            REPORT
            END
            """,
            contains=("first message", "second message", "both"),
            ordered=("first message", "second message"),
        ),
        case(
            "F003",
            "alias define/rename/erase/dump",
            """
            DEFINE old open file now
            EXPAND old
            RENAME old new
            EXPAND new
            ERASE old
            DUMP
            END
            """,
            contains=("open file now", "new"),
            ordered=("open file now", "open file now"),
        ),
        case(
            "F004",
            "template render and unset",
            """
            SET name Alice
            SET city Paris
            RENDER Hello {name} in {city} and {missing}
            UNSET city
            RENDER City is {city}
            VARS
            END
            """,
            contains=("Hello Alice in Paris and {missing}", "City is {city}", "name"),
            absent=("city Paris",),
        ),
        case(
            "F005",
            "line editor insert/patch/delete",
            """
            APPEND alpha one
            INSERT 0 beta zero
            PATCH 1 gamma two
            DELETE 0
            PRINT
            END
            """,
            contains=("gamma two",),
            absent=("beta zero", "alpha one"),
        ),
        case(
            "F006",
            "inventory add/take/count/report",
            """
            ADD screws 5
            ADD nails 3
            TAKE screws 2
            COUNT screws
            TAKE nails 10
            COUNT nails
            REPORT
            END
            """,
            contains=("screws", "nails"),
            regex=(r"\b3\b", r"\b0\b"),
        ),
        case(
            "F007",
            "activity log find/sort/delete",
            """
            ADD 20 beta later note
            ADD 10 alpha early note
            FIND alpha
            SORT
            DELETE 10 alpha
            SORT
            END
            """,
            contains=("alpha", "early note", "beta", "later note"),
            ordered=("10", "alpha", "20", "beta"),
        ),
        case(
            "F008",
            "gradebook update/drop/average",
            """
            ADD Ann 80 first note
            ADD Bob 100 second note
            UPDATE Ann 90 revised note
            AVERAGE
            DROP Bob
            LIST
            END
            """,
            contains=("Ann", "revised note"),
            regex=(r"\b95\b",),
        ),
        case(
            "F009",
            "grouped event merge/delete/count",
            """
            EVENT g1 first event
            EVENT g1 second event
            EVENT g2 third event
            COUNT g1
            MERGE g1 g2 both
            COUNT both
            DELETEGROUP g1
            REPORT
            END
            """,
            contains=("both",),
            regex=(r"\b2\b", r"\b3\b"),
        ),
        case(
            "F010",
            "name-to-list add/remove/values/keys",
            """
            ADD k v1
            ADD k v2
            ADD z zx
            REMOVE k v1
            VALUES k
            KEYS
            END
            """,
            contains=("v2", "k", "z"),
            absent=("v1",),
        ),
        case(
            "F011",
            "draft revise/discard/restore",
            """
            CREATE draft first body
            REVISE draft revised body
            FIND draft
            DISCARD draft
            RESTORE
            FIND draft
            PRINT
            END
            """,
            contains=("draft", "revised body"),
            ordered=("revised body", "revised body"),
        ),
        case(
            "F012",
            "checkout queue serve/cancel/rejoin",
            """
            JOIN Ann first note
            JOIN Bob second note
            FIND Bob
            SERVE
            QUEUE
            END
            """,
            contains=("Ann", "first note", "Bob", "second note"),
            ordered=("Bob", "second note", "Ann", "Bob"),
            hard_contains=("Ann", "first note", "second note"),
        ),
        case(
            "F013",
            "task mover todo/doing/done",
            """
            TODO build write code
            START build
            FIND build
            DONE build
            FIND build
            REPORT
            END
            """,
            contains=("build", "write code", "doing", "done"),
            ordered=("doing", "done"),
        ),
        case(
            "F014",
            "label tracker unlabel/delete",
            """
            ITEM item1 base note
            LABEL item1 red
            LABEL item1 blue
            UNLABEL item1 red
            FIND item1
            REPORT
            END
            """,
            contains=("item1", "base note", "blue"),
            absent=("red",),
        ),
        case(
            "F015",
            "contact note/merge/delete",
            """
            ADD a a@example.com 111
            NOTE a first note
            ADD b b@example.com 222
            NOTE b second note
            MERGE a b
            FIND a
            FIND b
            LIST
            END
            """,
            contains=("a@example.com", "111", "first note", "second note"),
            absent=("b@example.com",),
        ),
        case(
            "F016",
            "contact importer load/add/delete/report",
            """
            FIND Ann
            ADD Cara,cara@example.com,333
            DELETE Bob
            REPORT
            END
            """,
            files={
                "contacts.csv": (
                    "Ann,ann@example.com,111\n"
                    "badline\n"
                    "Bob,bob@example.com,222\n"
                )
            },
            argv=("contacts.csv",),
            contains=("Ann", "ann@example.com", "Cara", "cara@example.com"),
            absent=("badline", "Bob"),
        ),
        case(
            "F017",
            "config editor load/set/unset/write",
            """
            GET host
            SET host new value
            UNSET port
            WRITE
            END
            """,
            files={"config.txt": "host=old value\nport=8080\nmalformed\n"},
            argv=("config.txt",),
            contains=("old value", "host new value"),
            absent=("port 8080",),
        ),
        case(
            "F018",
            "word frequency load/remove/top",
            """
            WORD banana
            COUNT apple
            COUNT banana
            REMOVE apple
            TOP
            END
            """,
            files={"words.txt": "apple\nbanana\napple\n"},
            argv=("words.txt",),
            contains=("banana",),
            regex=(r"\b2\b",),
        ),
        case(
            "F019",
            "transaction journal balance/reverse",
            """
            BALANCE acct1
            ADD acct1 -3 memo_c
            BALANCE acct1
            REVERSE acct1 5
            BALANCE acct1
            REPORT
            END
            """,
            files={
                "journal.txt": (
                    "acct1 10 memo_a\n"
                    "acct1 5 memo_b\n"
                    "bad x y\n"
                )
            },
            argv=("journal.txt",),
            regex=(r"\b15\b", r"\b12\b", r"\b7\b"),
        ),
        case(
            "F020",
            "chunked catalog find/delete/add/print",
            """
            FIND Alpha
            DELETE Beta
            ADD Gamma
            new body
            --
            PRINT
            END
            """,
            files={"catalog.txt": "Alpha\none two\n--\nBeta\nthree\n--\n"},
            argv=("catalog.txt",),
            contains=("Alpha", "one two", "Gamma", "new body"),
            absent=("Beta", "three"),
        ),
        case(
            "F021",
            "recipe ingredient add/remove/show",
            """
            RECIPE soup
            ING soup salt fine grain
            ING soup water cold
            REMOVEING soup salt
            SHOW soup
            LIST
            END
            """,
            contains=("soup", "water", "cold"),
            absent=("salt", "fine grain"),
        ),
        case(
            "F022",
            "project task move/done/report",
            """
            PROJECT p1
            PROJECT p2
            TASK p1 t1 note one
            MOVETASK p1 p2 t1
            DONETASK p2 t1
            REPORT
            END
            """,
            contains=("p2", "t1", "note one", "done"),
        ),
        case(
            "F023",
            "mailbox tag/search/delete/report",
            """
            MAIL ann sub1 body one
            TAG sub1 work
            MAIL bob sub2 body two
            TAG sub2 home
            SEARCH work
            DELETE sub1
            REPORT
            END
            """,
            contains=("sub1", "work", "sub2", "home"),
            hard_contains=("sub1", "sub2", "home"),
        ),
        case(
            "F024",
            "bibliography author remove/find/list",
            """
            BOOK Book_A 2020
            AUTHOR Book_A Alice
            AUTHOR Book_A Bob
            REMOVEAUTHOR Book_A Alice
            FIND Book_A
            LIST
            END
            """,
            contains=("Book_A", "2020", "Bob"),
            hard_contains=("Book_A", "Bob"),
            absent=("Alice",),
        ),
        case(
            "F025",
            "classroom transfer/drop/print",
            """
            CLASS c1
            CLASS c2
            STUDENT c1 Ann note one
            TRANSFER c1 c2 Ann
            PRINT
            DROP c2 Ann
            PRINT
            END
            """,
            contains=("c2", "Ann", "note one"),
        ),
        case(
            "F026",
            "filtered notes find/keep/print",
            """
            ADD work fix bug
            ADD home wash dishes
            FIND bug
            KEEP work
            PRINT
            END
            """,
            contains=("work", "fix bug"),
            absent=("home", "wash dishes"),
        ),
        case(
            "F027",
            "priority reminders bump/top/remove",
            """
            ADD 5 t1 note one
            ADD 10 t2 note two
            BUMP t1 10
            TOP 1
            REMOVE t1
            LIST
            END
            """,
            contains=("t1", "note one", "t2", "note two"),
        ),
        case(
            "F028",
            "route planner insert/move/find/print",
            """
            ADD A first stop
            ADD B second stop
            INSERT 1 X middle stop
            MOVE 2 0
            FIND X
            PRINT
            END
            """,
            contains=("A", "B", "X", "middle stop"),
        ),
        case(
            "F029",
            "duplicate cleaner dedup/delete/find",
            """
            ADD k v1
            ADD k v2
            ADD z z1
            FIND k
            DEDUP
            FIND k
            DELETE z
            REPORT
            END
            """,
            contains=("k", "v1", "v2"),
            absent=("z1",),
        ),
        case(
            "F030",
            "rename table apply/swap/delete",
            """
            ADD cat dog
            ADD red blue
            APPLY the cat sat
            SWAP 0 1
            RULES
            DELETE red
            APPLY red cat
            END
            """,
            contains=("the dog sat", "red", "blue", "cat", "dog"),
            hard_contains=("red", "blue", "cat", "dog"),
        ),
        case(
            "F031",
            "batch importer all-or-nothing",
            """
            BATCH 2
            a note one
            b note two
            BATCH 2
            badonly
            c note three
            FIND a
            FIND c
            REPORT
            END
            """,
            contains=("a", "note one", "b", "note two"),
            absent=("note three",),
        ),
        case(
            "F032",
            "transactional settings abort/commit",
            """
            SET mode base
            BEGIN
            TSET mode staged
            TSET temp value
            ABORT
            GET mode
            GET temp
            BEGIN
            TSET mode final
            COMMIT
            DUMP
            END
            """,
            contains=("base", "final"),
            absent=("staged", "temp value"),
        ),
        case(
            "F033",
            "employee update/delete/list",
            """
            ADD 1 Alice Sales Dev
            UPDATE 1 Alice2 Eng Lead
            FIND 1
            DELETE 1
            LIST
            END
            """,
            contains=("1", "Alice2", "Eng", "Lead"),
            absent=("Sales",),
        ),
        case(
            "F034",
            "replace-from-file note store load",
            """
            ADD local local note
            REPORT
            LOAD
            REPORT
            DELETE file1
            REPORT
            END
            """,
            files={"notes.txt": "file1 file note\nfile2 second note\n"},
            argv=("notes.txt",),
            contains=("local note", "file1", "file note", "file2", "second note"),
        ),
        case(
            "F035",
            "copy queue clone/run/list",
            """
            ENQUEUE a payload one
            CLONE a b
            RUN b
            LIST
            CANCEL a
            LIST
            END
            """,
            contains=("payload one", "a"),
        ),
        case(
            "F036",
            "length-prefixed records",
            """
            ADD r1 5:hello
            ADD bad 10:short
            SHOW r1
            SHOW bad
            REPORT
            END
            """,
            contains=("hello", "r1"),
            absent=("short",),
        ),
        case(
            "F037",
            "hex buffer append/slice/list",
            """
            NEW buf 0A0B0C
            APPEND buf FF
            SLICE buf 1 2
            LIST
            END
            """,
            contains=("0B0C", "buf"),
            regex=(r"\b4\b",),
        ),
        case(
            "F038",
            "run-length decoder oversized reject",
            """
            STORE a 3x2y
            DECODE a
            STORE big 9999999999z
            LIST
            END
            """,
            contains=("xxxyy", "a"),
        ),
        case(
            "F039",
            "counted table set/get/delete",
            """
            TABLE t 2 2
            SET t 0 1 hello
            GET t 0 1
            GET t 1 1
            SET t 9 0 bad
            TABLES
            END
            """,
            contains=("hello", "EMPTY", "t"),
        ),
        case(
            "F040",
            "packet cache lengths/stats/drop",
            """
            PACKET p1 3 5 abc hello
            PACKET bad 4 2 abc zzz
            HEADER p1
            PAYLOAD p1
            STATS
            DROP p1
            STATS
            END
            """,
            contains=("abc", "hello"),
            regex=(r"\b5\b", r"\b0\b"),
        ),
    ]
    result = {c.task_id: c for c in cases}
    missing = {f"F{i:03d}" for i in range(1, 41)} - set(result)
    if missing:
        raise RuntimeError(f"missing functional cases: {sorted(missing)}")
    return result


def normalize_for_matching(text: str) -> str:
    text = text.replace("\r\n", "\n").replace("\r", "\n")
    text = text.replace("_", " ")
    text = text.lower()
    text = re.sub(r"[^a-z0-9{}@.+-]+", " ", text)
    text = re.sub(r"\s+", " ", text)
    return text.strip()


def phrase_present(output: str, phrase: str) -> bool:
    out = normalize_for_matching(output)
    exp = normalize_for_matching(phrase)
    if not exp:
        return True
    if exp in out:
        return True
    return exp.replace(" ", "") in out.replace(" ", "")


def phrases_in_order(output: str, phrases: Sequence[str]) -> bool:
    out = normalize_for_matching(output)
    pos = 0
    for phrase in phrases:
        exp = normalize_for_matching(phrase)
        idx = out.find(exp, pos)
        if idx < 0:
            return False
        pos = idx + len(exp)
    return True


def evaluate_checks(
    stdout: str,
    returncode: int,
    *,
    contains: Sequence[str] = (),
    absent: Sequence[str] = (),
    ordered: Sequence[str] = (),
    regex: Sequence[str] = (),
) -> Tuple[bool, List[str]]:
    failures: List[str] = []
    if returncode != 0:
        failures.append(f"nonzero_exit:{returncode}")
    for phrase in contains:
        if not phrase_present(stdout, phrase):
            failures.append(f"missing:{phrase}")
    for phrase in absent:
        if phrase_present(stdout, phrase):
            failures.append(f"unexpected:{phrase}")
    if ordered and not phrases_in_order(stdout, ordered):
        failures.append("order_mismatch:" + " -> ".join(ordered))
    for pattern in regex:
        if not re.search(pattern, stdout, flags=re.IGNORECASE | re.DOTALL):
            canon = normalize_for_matching(stdout)
            if not re.search(pattern, canon, flags=re.IGNORECASE | re.DOTALL):
                failures.append(f"regex_missing:{pattern}")
    return not failures, failures


def evaluate_output(case_obj: FunctionalCase, stdout: str, returncode: int) -> Tuple[bool, List[str]]:
    return evaluate_checks(
        stdout,
        returncode,
        contains=case_obj.contains,
        absent=case_obj.absent,
        ordered=case_obj.ordered,
        regex=case_obj.regex,
    )


def evaluate_hard_output(case_obj: FunctionalCase, stdout: str, returncode: int) -> Tuple[bool, List[str]]:
    return evaluate_checks(
        stdout,
        returncode,
        contains=case_obj.hard_contains,
        absent=case_obj.hard_absent,
        ordered=case_obj.hard_ordered,
        regex=case_obj.hard_regex,
    )


def classify_output(
    strict_passed: bool,
    strict_failures: Sequence[str],
    hard_passed: bool,
    hard_failures: Sequence[str],
) -> Tuple[str, str]:
    if strict_passed:
        return "test_match", ""
    if hard_passed:
        return (
            "acceptable_difference",
            "hard checks passed; strict test checks differed: " + ";".join(strict_failures),
        )
    return "implementation_error", "hard checks failed: " + ";".join(hard_failures)


def default_dataset_path() -> Optional[Path]:
    candidates = [
        Path(__file__).resolve().parents[1] / "Data Sheet" / "prompt_dataset.csv",
        Path.home() / "Desktop" / "Data Sheet" / "prompt_dataset.csv",
        Path("F:/Project Data/Data Sheet/prompt_dataset.csv"),
        Path.cwd() / "prompt_dataset.csv",
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate
    return None


def load_dataset(path: Optional[Path]) -> Dict[str, Dict[str, str]]:
    if not path or not path.exists():
        return {}
    with path.open("r", encoding="utf-8-sig", newline="") as f:
        rows = list(csv.DictReader(f))
    return {row.get("task_id", ""): row for row in rows if row.get("task_id")}


def find_sources(roots: Sequence[Path]) -> List[Tuple[Path, Path]]:
    found: List[Tuple[Path, Path]] = []
    for root in roots:
        root = root.resolve()
        if root.is_file() and root.suffix.lower() == ".c":
            found.append((root, root.parent))
            continue
        if not root.exists():
            print(f"warning: source root not found: {root}", file=sys.stderr)
            continue
        for src in root.rglob("*.c"):
            parts = {p.lower() for p in src.parts}
            if ".git" in parts or "__pycache__" in parts:
                continue
            if TASK_ID_RE.match(src.stem):
                found.append((src.resolve(), root))
    found.sort(key=lambda pair: str(pair[0]).lower())
    return found


def auto_experiment_roots(project_data: Path) -> List[Path]:
    roots: List[Path] = []
    methods = [
        "Baseline",
        "Simple Security Prompt",
        "General Secure C Principles",
        "Security Design First",
        "GSCP",
        "SDF",
    ]
    for model in ("Gemini", "Deepseek", "Qwen"):
        code_root = project_data / f"{model}_Code"
        for method in methods:
            candidate = code_root / method
            if candidate.exists():
                roots.append(candidate)
    return roots


def infer_labels(source: Path, source_root: Path) -> Dict[str, str]:
    parts = list(source.parts)
    lower_parts = [p.lower() for p in parts]
    model = ""
    prompt_type = ""
    run_id = ""

    for marker, model_name in (
        ("gemini_code", "Gemini"),
        ("deepseek_code", "Deepseek"),
        ("qwen_code", "Qwen"),
    ):
        if marker in lower_parts:
            idx = lower_parts.index(marker)
            model = model_name
            if idx + 1 < len(parts):
                prompt_type = parts[idx + 1]
            break

    if not model:
        joined = " ".join(lower_parts)
        if "gemini" in joined:
            model = "Gemini"
        elif "deepseek" in joined:
            model = "Deepseek"
        elif "qwen" in joined:
            model = "Qwen"

    if not prompt_type:
        root_name = source_root.name
        lname = root_name.lower()
        if "baseline" in lname:
            prompt_type = "Baseline"
        elif (
            "security_prompt" in lname
            or "security prompt" in lname
            or ("security" in lname and "simple" in lname)
        ):
            prompt_type = "Simple Security Prompt"
        elif "gscp" in lname or "general secure" in lname:
            prompt_type = "General Secure C Principles"
        elif "sdf" in lname or "security design first" in lname:
            prompt_type = "Security Design First"
        else:
            prompt_type = root_name

    for part in parts:
        m = re.search(r"(?:code|prompt|sdf|gscp|baseline|security)[_-]?(\d{2})", part, re.I)
        if m:
            run_id = m.group(1)
    return {
        "model": model,
        "prompt_type": prompt_type,
        "run_id": run_id,
        "source_label": source_root.name,
    }


def short_text(text: object, limit: int = 1200) -> str:
    if isinstance(text, bytes):
        text = text.decode("utf-8", errors="replace")
    elif text is None:
        text = ""
    else:
        text = str(text)
    text = text.replace("\r\n", "\n").replace("\r", "\n")
    if len(text) <= limit:
        return text
    return text[:limit] + "\n...[truncated]..."


def safe_name(path: Path) -> str:
    digest = hashlib.sha1(str(path).encode("utf-8", errors="replace")).hexdigest()[:12]
    return f"{path.stem}_{digest}"


def compile_source(source: Path, exe_path: Path, args: argparse.Namespace) -> Tuple[str, str, float]:
    compiler = args.compiler or os.environ.get("CC") or "gcc"
    if shutil.which(compiler) is None and not Path(compiler).exists():
        return "compiler_not_found", f"compiler not found: {compiler}", 0.0

    cmd = [
        compiler,
        "-std=gnu11",
        "-D_POSIX_C_SOURCE=200809L",
        "-D_GNU_SOURCE",
        "-Wall",
        "-Wextra",
        "-O0",
    ]
    if args.sanitize:
        cmd.extend(["-fsanitize=address,undefined", "-fno-omit-frame-pointer"])
    cmd.extend(args.cflag)
    cmd.extend([str(source), "-o", str(exe_path)])
    cmd.extend(args.ldflag)
    cmd.append("-lm")

    start = time.time()
    try:
        proc = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=args.compile_timeout,
    )
    except subprocess.TimeoutExpired as exc:
        elapsed = time.time() - start
        return "compile_timeout", short_text(short_text(exc.stdout) + short_text(exc.stderr)), elapsed
    elapsed = time.time() - start
    log = short_text((proc.stdout or "") + (proc.stderr or ""))
    if proc.returncode != 0:
        return "compile_failed", log, elapsed
    return "compiled", log, elapsed


def prepare_case_files(run_dir: Path, case_obj: FunctionalCase) -> List[str]:
    argv: List[str] = []
    for name, content in case_obj.files.items():
        file_path = run_dir / name
        file_path.parent.mkdir(parents=True, exist_ok=True)
        file_path.write_text(content, encoding="utf-8", newline="")
    for arg in case_obj.argv:
        if arg in case_obj.files:
            argv.append(str((run_dir / arg).resolve()))
        else:
            argv.append(arg)
    return argv


def run_case(exe_path: Path, case_obj: FunctionalCase, run_dir: Path, args: argparse.Namespace) -> Dict[str, str]:
    argv = prepare_case_files(run_dir, case_obj)
    cmd = [str(exe_path)] + argv
    start = time.time()
    try:
        proc = subprocess.run(
            cmd,
            input=case_obj.stdin,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=args.timeout,
            cwd=str(run_dir),
        )
        elapsed = time.time() - start
        passed, failures = evaluate_output(case_obj, proc.stdout, proc.returncode)
        hard_passed, hard_failures = evaluate_hard_output(case_obj, proc.stdout, proc.returncode)
        category, classification_reason = classify_output(
            passed, failures, hard_passed, hard_failures
        )
        return {
            "run_status": "passed" if passed else "failed",
            "passed": "1" if passed else "0",
            "test_match": "1" if category == "test_match" else "0",
            "hard_checks_passed": "1" if hard_passed else "0",
            "functional_category": category,
            "failure_reasons": ";".join(failures),
            "hard_failure_reasons": ";".join(hard_failures),
            "classification_reason": classification_reason,
            "returncode": str(proc.returncode),
            "runtime_sec": f"{elapsed:.3f}",
            "stdout": short_text(proc.stdout, args.capture_chars),
            "stderr": short_text(proc.stderr, args.capture_chars),
        }
    except subprocess.TimeoutExpired as exc:
        elapsed = time.time() - start
        return {
            "run_status": "timeout",
            "passed": "0",
            "test_match": "0",
            "hard_checks_passed": "0",
            "functional_category": "implementation_error",
            "failure_reasons": "timeout",
            "hard_failure_reasons": "timeout",
            "classification_reason": "runtime timeout",
            "returncode": "",
            "runtime_sec": f"{elapsed:.3f}",
            "stdout": short_text(exc.stdout or "", args.capture_chars),
            "stderr": short_text(exc.stderr or "", args.capture_chars),
        }


def process_source(
    source_pair: Tuple[Path, Path],
    cases: Dict[str, FunctionalCase],
    dataset: Dict[str, Dict[str, str]],
    build_root: Path,
    args: argparse.Namespace,
) -> List[Dict[str, str]]:
    source, source_root = source_pair
    task_id = source.stem.upper()
    labels = infer_labels(source, source_root)
    meta = dataset.get(task_id, {})
    base_row = {
        "task_id": task_id,
        "task_category": meta.get("task_category", ""),
        "primary_cwe": meta.get("primary_cwe", ""),
        "model": labels["model"],
        "prompt_type": labels["prompt_type"],
        "run_id": labels["run_id"],
        "source_label": labels["source_label"],
        "source_root": str(source_root),
        "source_file": str(source),
    }
    case_obj = cases.get(task_id)
    if not case_obj:
        row = dict(base_row)
        row.update(
            {
                "case_name": "",
                "compile_status": "no_case",
                "compile_sec": "",
                "compile_log": "",
                "run_status": "not_run",
                "passed": "0",
                "test_match": "0",
                "hard_checks_passed": "0",
                "functional_category": "implementation_error",
                "failure_reasons": "no_functional_case",
                "hard_failure_reasons": "no_functional_case",
                "classification_reason": "no embedded functional case",
                "returncode": "",
                "runtime_sec": "",
                "stdout": "",
                "stderr": "",
            }
        )
        return [row]

    source_build_dir = build_root / safe_name(source)
    source_build_dir.mkdir(parents=True, exist_ok=True)
    exe_suffix = ".exe" if os.name == "nt" else ""
    exe_path = source_build_dir / f"{source.stem}{exe_suffix}"
    compile_status, compile_log, compile_sec = compile_source(source, exe_path, args)

    row = dict(base_row)
    row.update(
        {
            "case_name": case_obj.name,
            "case_notes": case_obj.notes,
            "compile_status": compile_status,
            "compile_sec": f"{compile_sec:.3f}",
            "compile_log": compile_log,
        }
    )
    if compile_status != "compiled":
        row.update(
            {
                "run_status": "not_run",
                "passed": "0",
                "test_match": "0",
                "hard_checks_passed": "0",
                "functional_category": "implementation_error",
                "failure_reasons": compile_status,
                "hard_failure_reasons": compile_status,
                "classification_reason": "compile did not produce a runnable program",
                "returncode": "",
                "runtime_sec": "",
                "stdout": "",
                "stderr": "",
            }
        )
        return [row]

    run_dir = source_build_dir / "run"
    run_dir.mkdir(parents=True, exist_ok=True)
    row.update(run_case(exe_path, case_obj, run_dir, args))
    return [row]


def process_source_safe(
    source_pair: Tuple[Path, Path],
    cases: Dict[str, FunctionalCase],
    dataset: Dict[str, Dict[str, str]],
    build_root: Path,
    args: argparse.Namespace,
) -> List[Dict[str, str]]:
    try:
        return process_source(source_pair, cases, dataset, build_root, args)
    except subprocess.TimeoutExpired as exc:
        source, source_root = source_pair
        task_id = source.stem.upper()
        labels = infer_labels(source, source_root)
        meta = dataset.get(task_id, {})
        case_obj = cases.get(task_id)
        elapsed = getattr(exc, "timeout", args.timeout)
        return [
            {
                "task_id": task_id,
                "task_category": meta.get("task_category", ""),
                "primary_cwe": meta.get("primary_cwe", ""),
                "model": labels["model"],
                "prompt_type": labels["prompt_type"],
                "run_id": labels["run_id"],
                "source_label": labels["source_label"],
                "source_root": str(source_root),
                "source_file": str(source),
                "case_name": case_obj.name if case_obj else "",
                "case_notes": case_obj.notes if case_obj else "",
                "compile_status": "compiled",
                "compile_sec": "",
                "compile_log": "",
                "run_status": "timeout",
                "passed": "0",
                "test_match": "0",
                "hard_checks_passed": "0",
                "functional_category": "implementation_error",
                "failure_reasons": "timeout",
                "hard_failure_reasons": "timeout",
                "classification_reason": "runtime timeout; recovered by outer safety handler",
                "returncode": "",
                "runtime_sec": f"{float(elapsed):.3f}" if elapsed else "",
                "stdout": short_text(exc.stdout, args.capture_chars),
                "stderr": short_text(exc.stderr, args.capture_chars),
            }
        ]
    except Exception as exc:
        source, source_root = source_pair
        task_id = source.stem.upper()
        labels = infer_labels(source, source_root)
        meta = dataset.get(task_id, {})
        case_obj = cases.get(task_id)
        reason = f"internal_error:{type(exc).__name__}:{short_text(exc, 300)}"
        return [
            {
                "task_id": task_id,
                "task_category": meta.get("task_category", ""),
                "primary_cwe": meta.get("primary_cwe", ""),
                "model": labels["model"],
                "prompt_type": labels["prompt_type"],
                "run_id": labels["run_id"],
                "source_label": labels["source_label"],
                "source_root": str(source_root),
                "source_file": str(source),
                "case_name": case_obj.name if case_obj else "",
                "case_notes": case_obj.notes if case_obj else "",
                "compile_status": "internal_error",
                "compile_sec": "",
                "compile_log": reason,
                "run_status": "not_run",
                "passed": "0",
                "test_match": "0",
                "hard_checks_passed": "0",
                "functional_category": "implementation_error",
                "failure_reasons": reason,
                "hard_failure_reasons": reason,
                "classification_reason": "script recovered from an unexpected per-file error",
                "returncode": "",
                "runtime_sec": "",
                "stdout": "",
                "stderr": "",
            }
        ]


def summarize(rows: Sequence[Dict[str, str]]) -> str:
    summary = summary_rows(rows)
    if not summary:
        return "Functional summary\n(no rows)"
    fieldnames = [
        "model",
        "prompt_type",
        "run_id",
        "source_label",
        "files",
        "compiled",
        "compile_failed",
        "not_run",
        "test_match",
        "acceptable_difference",
        "implementation_error",
        "timeout",
        "test_match_rate",
        "acceptable_behavior_rate",
        "implementation_error_rate",
    ]
    lines = ["Functional summary", ",".join(fieldnames)]
    for row in summary:
        lines.append(",".join(str(row.get(name, "")) for name in fieldnames))
    return "\n".join(lines)


def summary_rows(rows: Sequence[Dict[str, str]]) -> List[Dict[str, str]]:
    groups: Dict[Tuple[str, str, str], List[Dict[str, str]]] = {}
    for row in rows:
        key = (
            row.get("model", ""),
            row.get("prompt_type", ""),
            row.get("source_label", ""),
        )
        groups.setdefault(key, []).append(row)
    summary: List[Dict[str, str]] = []
    for key in sorted(groups):
        group = groups[key]
        total = len(group)
        compiled = sum(1 for r in group if r.get("compile_status") == "compiled")
        compile_failed = sum(1 for r in group if r.get("compile_status") != "compiled")
        not_run = sum(1 for r in group if r.get("run_status") == "not_run")
        test_match = sum(1 for r in group if r.get("functional_category") == "test_match")
        acceptable_difference = sum(
            1 for r in group if r.get("functional_category") == "acceptable_difference"
        )
        implementation_error = sum(
            1 for r in group if r.get("functional_category") == "implementation_error"
        )
        timeout = sum(1 for r in group if r.get("run_status") == "timeout")
        test_match_rate = test_match / total if total else 0.0
        acceptable_behavior_rate = (
            (test_match + acceptable_difference) / total if total else 0.0
        )
        implementation_error_rate = implementation_error / total if total else 0.0
        run_ids = sorted({r.get("run_id", "") for r in group if r.get("run_id", "")})
        summary.append(
            {
                "model": key[0],
                "prompt_type": key[1],
                "run_id": "+".join(run_ids),
                "source_label": key[2],
                "files": str(total),
                "compiled": str(compiled),
                "compile_failed": str(compile_failed),
                "not_run": str(not_run),
                "test_match": str(test_match),
                "acceptable_difference": str(acceptable_difference),
                "implementation_error": str(implementation_error),
                "timeout": str(timeout),
                "test_match_rate": f"{test_match_rate:.3f}",
                "acceptable_behavior_rate": f"{acceptable_behavior_rate:.3f}",
                "implementation_error_rate": f"{implementation_error_rate:.3f}",
            }
        )
    return summary


DETAIL_FIELDNAMES = [
    "task_id",
    "task_category",
    "primary_cwe",
    "model",
    "prompt_type",
    "run_id",
    "source_label",
    "source_root",
    "source_file",
    "case_name",
    "case_notes",
    "compile_status",
    "compile_sec",
    "compile_log",
    "run_status",
    "passed",
    "test_match",
    "hard_checks_passed",
    "functional_category",
    "failure_reasons",
    "hard_failure_reasons",
    "classification_reason",
    "returncode",
    "runtime_sec",
    "stdout",
    "stderr",
]


FUNCTIONAL_SUMMARY_FIELDNAMES = [
    "task_id",
    "task_category",
    "case_name",
    "model",
    "prompt_type",
    "run_id",
    "source_label",
    "compile_status",
    "run_status",
    "passed",
    "test_match",
    "hard_checks_passed",
    "functional_category",
    "failure_reasons",
    "hard_failure_reasons",
    "classification_reason",
    "compile_sec",
    "runtime_sec",
    "source_file",
]


SUMMARY_FIELDNAMES = [
    "model",
    "prompt_type",
    "run_id",
    "source_label",
    "files",
    "compiled",
    "compile_failed",
    "not_run",
    "test_match",
    "acceptable_difference",
    "implementation_error",
    "timeout",
    "test_match_rate",
    "acceptable_behavior_rate",
    "implementation_error_rate",
]


def write_csv(rows: Sequence[Dict[str, str]], out_path: Path) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=DETAIL_FIELDNAMES, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def write_table_csv(rows: Sequence[Dict[str, str]], out_path: Path, fieldnames: Sequence[str]) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def functional_summary_rows(rows: Sequence[Dict[str, str]]) -> List[Dict[str, str]]:
    result: List[Dict[str, str]] = []
    for row in rows:
        result.append({name: row.get(name, "") for name in FUNCTIONAL_SUMMARY_FIELDNAMES})
    return result


def normalize_out_path(out_path: Path) -> Path:
    """Accept either a CSV file path or a result directory path."""
    if out_path.exists() and out_path.is_dir():
        return out_path / "functional_results.csv"
    if out_path.suffix.lower() == ".csv":
        return out_path
    return out_path / "functional_results.csv"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    default_dataset = default_dataset_path()
    parser = argparse.ArgumentParser(
        description="Compile and functionally test F001.c..F040.c benchmark solutions."
    )
    parser.add_argument(
        "--src",
        action="append",
        default=[],
        help="C source directory or single .c file. Repeatable.",
    )
    parser.add_argument(
        "--project-data",
        type=Path,
        default=None,
        help="Project Data root. Use with --auto-experiments to scan standard code folders.",
    )
    parser.add_argument(
        "--auto-experiments",
        action="store_true",
        help=(
            "Scan standard Gemini_Code/Deepseek_Code/Qwen_Code method folders "
            "under --project-data."
        ),
    )
    parser.add_argument(
        "--dataset",
        type=Path,
        default=default_dataset,
        help="prompt_dataset.csv path for metadata. Cases are embedded in the script.",
    )
    parser.add_argument(
        "--out",
        type=Path,
        default=Path("functional_results.csv"),
        help=(
            "Output CSV file, or a result directory. If a directory/no suffix is "
            "provided, functional_results.csv is written inside it."
        ),
    )
    parser.add_argument(
        "--recursive",
        action="store_true",
        help="Compatibility option. Source scanning is recursive by default.",
    )
    parser.add_argument("--compiler", default=os.environ.get("CC", "gcc"))
    parser.add_argument("--cflag", action="append", default=[], help="Extra C compiler flag.")
    parser.add_argument("--ldflag", action="append", default=[], help="Extra linker flag.")
    parser.add_argument("--sanitize", action="store_true", help="Compile with ASan/UBSan.")
    parser.add_argument("--timeout", type=float, default=5.0, help="Per-case runtime timeout.")
    parser.add_argument("--compile-timeout", type=float, default=20.0)
    parser.add_argument("--capture-chars", type=int, default=1200)
    parser.add_argument("--jobs", type=int, default=1, help="Parallel compile/run workers.")
    parser.add_argument("--keep-build", action="store_true")
    parser.add_argument("--build-dir", type=Path, default=Path(".functional_build"))
    parser.add_argument("--task", action="append", default=[], help="Only run specific task id(s).")
    parser.add_argument("--list-cases", action="store_true", help="Print embedded cases and exit.")
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    cases = build_cases()
    if args.task:
        requested = {t.upper() for t in args.task}
        cases = {task_id: c for task_id, c in cases.items() if task_id in requested}

    dataset = load_dataset(args.dataset)
    if not dataset:
        print(
            "warning: prompt_dataset.csv was not found or was empty; "
            "task_category and primary_cwe metadata will be blank",
            file=sys.stderr,
        )
    if args.list_cases:
        for task_id in sorted(cases):
            c = cases[task_id]
            category = dataset.get(task_id, {}).get("task_category", "")
            print(f"{task_id}\t{category}\t{c.name}")
        return 0

    roots = [Path(p) for p in args.src]
    if args.auto_experiments:
        project_data = args.project_data or Path("F:/Project Data")
        roots.extend(auto_experiment_roots(project_data))
    if not roots:
        roots = [Path.cwd()]

    sources = find_sources(roots)
    if args.task:
        wanted = set(cases)
        sources = [(src, root) for src, root in sources if src.stem.upper() in wanted]
    if not sources:
        print("No F001.c..F040.c sources found. Pass --src or --auto-experiments.", file=sys.stderr)
        return 2

    args.out = normalize_out_path(args.out)
    print(f"Dataset: {args.dataset if args.dataset else '(none)'}")
    print(f"Sources: {len(sources)}")
    print(f"Functional results: {args.out}")

    rows: List[Dict[str, str]] = []
    if args.keep_build:
        build_root = args.build_dir.resolve()
        build_root.mkdir(parents=True, exist_ok=True)
        cleanup_context = None
    else:
        cleanup_context = tempfile.TemporaryDirectory(prefix="functional_c_")
        build_root = Path(cleanup_context.name)

    try:
        if args.jobs > 1:
            with ThreadPoolExecutor(max_workers=args.jobs) as executor:
                futures = [
                    executor.submit(process_source_safe, pair, cases, dataset, build_root, args)
                    for pair in sources
                ]
                for future in as_completed(futures):
                    rows.extend(future.result())
        else:
            for pair in sources:
                rows.extend(process_source_safe(pair, cases, dataset, build_root, args))
    finally:
        if cleanup_context is not None:
            cleanup_context.cleanup()

    rows.sort(key=lambda r: (r.get("source_root", ""), r.get("source_file", "")))
    write_table_csv(functional_summary_rows(rows), args.out, FUNCTIONAL_SUMMARY_FIELDNAMES)
    print(summarize(rows))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
