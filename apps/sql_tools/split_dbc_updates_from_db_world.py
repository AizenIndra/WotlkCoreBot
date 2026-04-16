#!/usr/bin/env python3
"""
Move SQL statements that only touch *_dbc tables from data/sql/updates/db_world/*.sql
into data/sql/updates/db_dbc/<same filename>.

- If the entire file is DBC-only, moves the whole file to db_dbc.
- If mixed, extracts DBC-only statements into db_dbc and removes them from db_world.

Run from repo root: python apps/sql_tools/split_dbc_updates_from_db_world.py
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DB_WORLD = ROOT / "data/sql/updates/db_world"
DB_DBC = ROOT / "data/sql/updates/db_dbc"

# Match `tablename` in common SQL verbs (not exhaustive but enough for our updates)
TABLE_RE = re.compile(
    r"\b(?:FROM|INTO|UPDATE|JOIN|TABLE)\s+`([^`]+)`",
    re.IGNORECASE,
)


def tables_in_statement(stmt: str) -> set[str]:
    return set(TABLE_RE.findall(stmt))


def is_dbc_table(name: str) -> bool:
    return name.endswith("_dbc") or name in ("spell_dbc", "spelldifficulty_dbc")


def split_statements(sql: str) -> list[str]:
    """Split on semicolons that end a statement (heuristic: ; followed by newline or end)."""
    parts: list[str] = []
    buf: list[str] = []
    for line in sql.splitlines(keepends=True):
        buf.append(line)
        stripped = line.rstrip()
        if stripped.endswith(";"):
            parts.append("".join(buf))
            buf = []
    tail = "".join(buf).strip()
    if tail:
        parts.append(tail if tail.endswith(";") else tail + ";\n")
    return parts


def classify_file(path: Path) -> tuple[list[str], list[str]]:
    text = path.read_text(encoding="utf-8", errors="replace")
    stmts = split_statements(text)
    dbc_stmts: list[str] = []
    world_stmts: list[str] = []
    for s in stmts:
        s = s.strip()
        if not s or s.startswith("--"):
            # keep standalone comments with following world block
            if not s:
                continue
            # comment-only chunks: attach to next statement by keeping in original flow
            # Simpler: treat as world if no table refs
            tnames = tables_in_statement(s)
            if not tnames:
                world_stmts.append(s)
                continue
        tnames = tables_in_statement(s)
        if not tnames:
            world_stmts.append(s)
            continue
        if all(is_dbc_table(t) for t in tnames):
            dbc_stmts.append(s)
        elif any(is_dbc_table(t) for t in tnames):
            # Mixed references in one statement — manual fix required
            print(f"MIXED (manual): {path.name}\n{s[:200]}...", file=sys.stderr)
            world_stmts.append(s)
        else:
            world_stmts.append(s)
    return world_stmts, dbc_stmts


def main() -> int:
    DB_DBC.mkdir(parents=True, exist_ok=True)
    moved_whole = 0
    split_count = 0
    for path in sorted(DB_WORLD.glob("*.sql")):
        world_stmts, dbc_stmts = classify_file(path)
        if not dbc_stmts:
            continue
        dbc_out = "\n\n".join(x.rstrip() for x in dbc_stmts if x.strip()) + "\n"
        header = (
            f"-- Split from db_world/{path.name}: DBC-only statements (acore_dbc)\n\n"
        )
        if not world_stmts:
            # full move
            (DB_DBC / path.name).write_text(header + dbc_out, encoding="utf-8")
            path.unlink()
            moved_whole += 1
            print(f"MOVED whole -> db_dbc/{path.name}")
            continue
        # partial: append or create db_dbc file
        target = DB_DBC / path.name
        if target.exists():
            target.write_text(header + dbc_out, encoding="utf-8")
        else:
            target.write_text(header + dbc_out, encoding="utf-8")
        new_world = "\n\n".join(x.rstrip() for x in world_stmts if x.strip()) + "\n"
        path.write_text(new_world, encoding="utf-8")
        split_count += 1
        print(f"SPLIT {path.name} -> db_dbc/{path.name} ({len(dbc_stmts)} stmt)")
    print(f"Done: moved_whole={moved_whole}, split={split_count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
