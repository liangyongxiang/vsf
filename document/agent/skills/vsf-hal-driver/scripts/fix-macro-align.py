#!/usr/bin/env python3
"""Fix backslash alignment in multi-line #define macros.

Standard: continuation backslash aligned to column 81
(content length 80 + '\\' at column 81).

For lines with content > 80 chars, try to reduce padding around '='.
If still > 80, report for manual fix.
"""

import sys, os, re

TARGET_COL = 81  # backslash at column 81
MAX_CONTENT = 80

# Pattern: field = value with 2+ spaces around =
# Reduce to single space around =
_EQ_PAD_RE = re.compile(r'(\S)\s{2,}=\s{2,}(\S)')
_EQ_PAD_RE2 = re.compile(r'(\S)\s{2,}=\s(\S)')
_EQ_PAD_RE3 = re.compile(r'(\S)\s=\s{2,}(\S)')

def fix_line(line, filename, linenum):
    """Fix a single continuation line. Returns (new_line, ok, msg)."""
    stripped = line.rstrip('\n\r')
    if not stripped.rstrip().endswith('\\'):
        return line, True, None

    # Strip trailing backslash and spaces
    content = stripped.rstrip().rstrip('\\').rstrip()
    content_len = len(content)

    if content_len > MAX_CONTENT:
        # Try reducing padding around '='
        reduced = _EQ_PAD_RE.sub(r'\1 = \2', content)
        if reduced == content:
            reduced = _EQ_PAD_RE2.sub(r'\1 = \2', content)
        if reduced == content:
            reduced = _EQ_PAD_RE3.sub(r'\1 = \2', content)

        if len(reduced) <= MAX_CONTENT:
            content = reduced
            content_len = len(content)
        else:
            return line, False, f"content still {len(reduced)} > {MAX_CONTENT} chars after reducing '=' padding"

    # Pad to target column
    padding_needed = TARGET_COL - content_len - 1  # -1 for backslash itself
    if padding_needed < 0:
        return line, False, f"content {content_len} > {MAX_CONTENT} even after fixes"

    new_line = content + ' ' * padding_needed + '\\' + '\n'
    return new_line, True, None


def fix_file(filepath):
    with open(filepath, 'r') as f:
        lines = f.readlines()

    fixed = 0
    errors = []
    new_lines = []

    for i, line in enumerate(lines, 1):
        stripped = line.rstrip('\n\r')
        if stripped.rstrip().endswith('\\'):
            new_line, ok, msg = fix_line(line, filepath, i)
            if not ok:
                errors.append((filepath, i, msg, line.rstrip('\n\r')))
            elif new_line != line:
                fixed += 1
            new_lines.append(new_line)
        else:
            new_lines.append(line)

    if fixed or errors:
        with open(filepath, 'w') as f:
            f.writelines(new_lines)

    return fixed, errors


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <file.c> [file2.c ...]")
        sys.exit(1)

    total_fixed = 0
    all_errors = []

    for path in sys.argv[1:]:
        fixed, errors = fix_file(path)
        total_fixed += fixed
        all_errors.extend(errors)
        if fixed:
            print(f"  {os.path.basename(path)}: fixed {fixed} lines")
        elif not errors:
            print(f"  {os.path.basename(path)}: OK")

    if all_errors:
        print("\nERRORS (manual fix required):")
        for filepath, line, msg, content in all_errors:
            print(f"  {os.path.basename(filepath)}:{line}: {msg}")
            print(f"    {content!r}")

    if total_fixed == 0 and not all_errors:
        print("\nAll files already aligned.")
    elif not all_errors:
        print(f"\nFixed {total_fixed} lines total.")
    else:
        print(f"\nFixed {total_fixed} lines, {len(all_errors)} errors remain.")
        sys.exit(1)


if __name__ == '__main__':
    main()
