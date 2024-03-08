import subprocess
import argparse
import re
import sys
import shutil

LLVM_OBJDUMP = "llvm-objdump"
LLVM_MCA = "llvm-mca"

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def run_llvm_objdump(file, symbols):
    args = [LLVM_OBJDUMP, "-d", "--no-addresses", "--no-show-raw-insn", "--symbolize-operands", file]

    if len(symbols) > 0:
        args.append(f"--disassemble-symbols={','.join(symbols)}")

    return subprocess.run(args, capture_output=True, text=True)

def run_llvm_mca(assembly, syntax, all_stats, all_views):
    args = [LLVM_MCA]

    if syntax == 'intel':
        args.append("-output-asm-variant=1")
    elif syntax == 'at&t':
        args.append("-output-asm-variant=0")
    else:
        raise ValueError("syntax is not 'intel' or 'at&t'")
    
    if all_stats:
        args.append("-all-stats")

    if all_views:
        args.append("-all-views")

    return subprocess.run(args, input=assembly, encoding='ascii', capture_output=True, text=True)

def string_split_args(val):
    return [s.strip() for s in val.split(",")]

def main():
    parser = argparse.ArgumentParser(description=
        "Runs 'llvm-mca' over compiled binary file."
    )
    parser.add_argument("file")
    parser.add_argument("-s", "--symbols", type=string_split_args, default=[])
    parser.add_argument("--syntax", choices=["intel", "at&t"], default="intel")
    parser.add_argument("--all-stats", action="store_true", default=False)
    parser.add_argument("--all-views", action="store_true", default=False)

    args = parser.parse_args()

    try:
        output = run_llvm_objdump(args.file, args.symbols)
    except FileNotFoundError:
        eprint(f"Tool '{LLVM_OBJDUMP}' not found")
        sys.exit(1)
    
    if output.returncode != 0:
        eprint(f"Failed to run 'llvm-objdump': {output.stderr}")
        sys.exit(1)

    clean_assembly = re.sub("[<>]", "", output.stdout)

    try:
        analyzed_output = run_llvm_mca(clean_assembly, args.syntax, args.all_stats, args.all_views)
    except FileNotFoundError:
        eprint(f"Tool '{LLVM_MCA}' not found")
        sys.exit(1)
    
    if analyzed_output.returncode != 0:
        eprint(f"Failed to run 'llvm-mca': {analyzed_output.stderr}")
        sys.exit(1)

    print(analyzed_output.stdout)

if __name__ == "__main__":
    main()

