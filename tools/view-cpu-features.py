import subprocess
import sys
import re
import argparse
import os

LLVM_OBJDUMP = "llvm-objdump"

CPU_FEATURES_TABLE = {
    "BMI2": ("BZHI", "MULX", "PDEP", "PEXT", "RORX", "SARX", "SHRX", "SHLX"),
    "BMI1": ("ANDN", "BEXTR", "BLSI", "BLSMSK", "BLSR", "TZCNT"),
    "POPCNT": ("POPCNT", ),
    "LZCNT": ("LZCNT", ),
    "AVX2": ("vpabsw", "vpabsd", "vpabsb", "vpaddw", "vpaddd", "vpaddq", "vpaddb", "vpaddsw", "vpaddsb", "vpaddusw", "vpaddusb", "vpalignr", "vpand", "vpandn", "vpavgw", "vpavgb", "vpblendw", "vpblendd", "vpblendd", "vpblendvb", "vpbroadcastb", "vpbroadcastb", "vpbroadcastd", "vpbroadcastd", "vpbroadcastq", "vpbroadcastq", "movddup", "vbroadcastsd", "vbroadcasti128", "vbroadcasti128", "vbroadcastss", "vbroadcastss", "vpbroadcastw", "vpbroadcastw", "vpslldq", "vpsrldq", "vpcmpeqw", "vpcmpeqd", "vpcmpeqq", "vpcmpeqb", "vpcmpgtw", "vpcmpgtd", "vpcmpgtq", "vpcmpgtb", "vpmovsxwd", "vpmovsxwq", "vpmovsxdq", "vpmovsxbw", "vpmovsxbd", "vpmovsxbq", "vpmovzxwd", "vpmovzxwq", "vpmovzxdq", "vpmovzxbw", "vpmovzxbd", "vpmovzxbq", "vextracti128", "vphaddw", "vphaddd", "vphaddsw", "vphsubw", "vphsubd", "vphsubsw", "vpgatherdd", "vpgatherdd", "vpgatherdd", "vpgatherdd", "vpgatherdq", "vpgatherdq", "vpgatherdq", "vpgatherdq", "vgatherdpd", "vgatherdpd", "vgatherdpd", "vgatherdpd", "vgatherdps", "vgatherdps", "vgatherdps", "vgatherdps", "vpgatherqd", "vpgatherqd", "vpgatherqd", "vpgatherqd", "vpgatherqq", "vpgatherqq", "vpgatherqq", "vpgatherqq", "vgatherqpd", "vgatherqpd", "vgatherqpd", "vgatherqpd", "vgatherqps", "vgatherqps", "vgatherqps", "vgatherqps", "vinserti128", "vpmaddwd", "vpmaddubsw", "vpmaskmovd", "vpmaskmovd", "vpmaskmovq", "vpmaskmovq", "vpmaskmovd", "vpmaskmovd", "vpmaskmovq", "vpmaskmovq", "vpmaxsw", "vpmaxsd", "vpmaxsb", "vpmaxuw", "vpmaxud", "vpmaxub", "vpminsw", "vpminsd", "vpminsb", "vpminuw", "vpminud", "vpminub", "vpmovmskb", "vmpsadbw", "vpmuldq", "vpmuludq", "vpmulhw", "vpmulhuw", "vpmulhrsw", "vpmullw", "vpmulld", "vpor", "vpacksswb", "vpackssdw", "vpackuswb", "vpackusdw", "vperm2i128", "vpermq", "vpermpd", "vpermd", "vpermps", "vpsadbw", "vpshufd", "vpshufb", "vpshufhw", "vpshuflw", "vpsignw", "vpsignd", "vpsignb", "vpsllw", "vpslld", "vpsllq", "vpsllw", "vpslld", "vpsllq", "vpslldq", "vpsllvd", "vpsllvd", "vpsllvq", "vpsllvq", "vpsraw", "vpsrad", "vpsraw", "vpsrad", "vpsravd", "vpsravd", "vpsrlw", "vpsrld", "vpsrlq", "vpsrlw", "vpsrld", "vpsrlq", "vpsrldq", "vpsrlvd", "vpsrlvd", "vpsrlvq", "vpsrlvq", "vmovntdqa", "vpsubw", "vpsubd", "vpsubq", "vpsubb", "vpsubsw", "vpsubsb", "vpsubusw", "vpsubusb", "vpunpckhwd", "vpunpckhdq", "vpunpckhqdq", "vpunpckhbw", "vpunpcklwd", "vpunpckldq", "vpunpcklqdq", "vpunpcklbw", "vpxor"),
    "AVX": ("vaddpd", "vaddps", "vaddsubpd", "vaddsubps", "vandpd", "vandps", "vandnpd", "vandnps", "vblendpd", "vblendps", "vblendvpd", "vblendvps", "vbroadcastf128", "vbroadcastf128", "vbroadcastsd", "vbroadcastss", "vbroadcastss", "vroundpd", "vroundps", "vcmppd", "vcmppd", "vcmpps", "vcmpps", "vcmpsd", "vcmpss", "vcvtdq2pd", "vcvtdq2ps", "vcvtpd2dq", "vcvtpd2ps", "vcvtps2dq", "vcvtps2pd", "vmovsd", "vmovd", "vmovss", "vcvttpd2dq", "vcvttps2dq", "vdivpd", "vdivps", "vdpps", "vextractf128", "vextractf128", "vextractf128", "vroundpd", "vroundps", "vhaddpd", "vhaddps", "vhsubpd", "vhsubps", "vinsertf128", "vinsertf128", "vinsertf128", "vlddqu", "vmovapd", "vmovaps", "vmovdqa", "vmovupd", "vmovups", "vmovdqu", "vmaskmovpd", "vmaskmovpd", "vmaskmovps", "vmaskmovps", "vmaskmovpd", "vmaskmovpd", "vmaskmovps", "vmaskmovps", "vmaxpd", "vmaxps", "vminpd", "vminps", "vmovddup", "vmovshdup", "vmovsldup", "vmovmskpd", "vmovmskps", "vmulpd", "vmulps", "vorpd", "vorps", "vpermilpd", "vpermilpd", "vpermilps", "vpermilps", "vperm2f128", "vperm2f128", "vperm2f128", "vpermilpd", "vpermilpd", "vpermilps", "vpermilps", "vrcpps", "vroundpd", "vroundps", "vrsqrtps", "vinsertf128", "vinsertf128", "vinsertf128", "vinsertf128", "vinsertf128", "vinsertf128", "vxorpd", "vxorps", "vpxor", "vshufpd", "vshufps", "vsqrtpd", "vsqrtps", "vmovapd", "vmovaps", "vmovdqa", "vmovupd", "vmovups", "vmovdqu", "vmovntpd", "vmovntps", "vmovntdq", "vsubpd", "vsubps", "vtestpd", "vtestpd", "vtestps", "vtestps", "vptest", "vtestpd", "vtestpd", "vtestps", "vtestps", "vptest", "vtestpd", "vtestpd", "vtestps", "vtestps", "vptest", "vunpckhpd", "vunpckhps", "vunpcklpd", "vunpcklps", "vxorpd", "vxorps", "vzeroall", "vzeroupper"),
    "SSE4.2:": ("pcmpestri", "pcmpestri", "pcmpestri", "pcmpestrm", "pcmpestri", "pcmpestri", "pcmpestri", "pcmpgtq", "pcmpistri", "pcmpistri", "pcmpistri", "pcmpistrm", "pcmpistri", "pcmpistri", "pcmpistri", "crc32", "crc32", "crc32", "crc32"),
    "SSE4.1": ("pblendw", "blendpd", "blendps", "pblendvb", "blendvpd", "blendvps", "roundpd", "roundps", "roundsd", "roundss", "pcmpeqq", "pmovsxwd", "pmovsxwq", "pmovsxdq", "pmovsxbw", "pmovsxbd", "pmovsxbq", "pmovzxwd", "pmovzxwq", "pmovzxdq", "pmovzxbw", "pmovzxbd", "pmovzxbq", "dppd", "dpps", "pextrd", "pextrq", "pextrb", "extractps", "roundpd", "roundps", "roundsd", "roundss", "pinsrd", "pinsrq", "pinsrb", "insertps", "pmaxsd", "pmaxsb", "pmaxuw", "pmaxud", "pminsd", "pminsb", "pminuw", "pminud", "phminposuw", "mpsadbw", "pmuldq", "pmulld", "packusdw", "roundpd", "roundps", "roundsd", "roundss", "movntdqa", "ptest", "ptest", "ptest", "ptest", "ptest"),
    "SSSE3": ("pabsw", "pabsd", "pabsb", "pabsw", "pabsd", "pabsb", "palignr", "palignr", "phaddw", "phaddd", "phaddw", "phaddw", "phaddsw", "phaddsw", "phsubw", "phsubd", "phsubw", "phsubd", "phsubsw", "phsubsw", "pmaddubsw", "pmaddubsw", "pmulhrsw", "pmulhrsw", "pshufb", "pshufb", "psignw", "psignd", "psignb", "psignw", "psignd", "psignb"),
    "SSE3": ("addsubpd", "addsubps", "haddpd", "haddps", "hsubpd", "hsubps", "lddqu", "movddup", "movddup", "movshdup", "movsldup"),
    "SSE2": ("paddw", "paddd", "paddq", "paddb", "addpd", "addsd", "paddq", "paddsw", "paddsb", "paddusw", "paddusb", "andpd", "pand", "andnpd", "pandn", "pavgw", "pavgb", "pslldq", "psrldq", "clflush", "pcmpeqw", "pcmpeqd", "pcmpeqb", "cmppd", "cmpsd", "cmppd", "cmpsd", "pcmpgtw", "pcmpgtd", "pcmpgtb", "cmppd", "cmpsd", "cmppd", "cmpsd", "pcmpgtw", "pcmpgtd", "pcmpgtb", "cmppd", "cmpsd", "cmppd", "cmpsd", "cmppd", "cmpsd", "cmppd", "cmpsd", "cmppd", "cmpsd", "cmppd", "cmpsd", "cmppd", "cmpsd", "cmppd", "cmpsd", "comisd", "comisd", "comisd", "comisd", "comisd", "comisd", "cvtdq2pd", "cvtdq2ps", "cvtpd2dq", "cvtpd2pi", "cvtpd2ps", "cvtpi2pd", "cvtps2dq", "cvtps2pd", "movsd", "cvtsd2si", "cvtsd2si", "cvtsd2si", "cvtsd2ss", "movd", "movq", "movq", "cvtsi2sd", "movd", "cvtsi2sd", "movq", "cvtsi2sd", "movq", "cvtss2sd", "cvttpd2dq", "cvttpd2pi", "cvttps2dq", "cvttsd2si", "cvttsd2si", "cvttsd2si", "divpd", "divsd", "pextrw", "pinsrw", "lfence", "movapd", "movsd", "movdqa", "movhpd", "movq", "movlpd", "movupd", "movdqu", "movd", "pmaddwd", "maskmovdqu", "pmaxsw", "pmaxub", "maxpd", "maxsd", "mfence", "pminsw", "pminub", "minpd", "minsd", "movq", "movsd", "pmovmskb", "movmskpd", "movdq2q", "movq2dq", "pmuludq", "mulpd", "mulsd", "pmuludq", "pmulhw", "pmulhuw", "pmullw", "orpd", "por", "packsswb", "packssdw", "packuswb", "pause", "psadbw", "xorpd", "pxor", "pshufd", "shufpd", "pshufhw", "pshuflw", "psllw", "pslld", "psllq", "psllw", "pslld", "psllq", "pslldq", "sqrtpd", "sqrtsd", "psraw", "psrad", "psraw", "psrad", "psrlw", "psrld", "psrlq", "psrlw", "psrld", "psrlq", "psrldq", "movapd", "movsd", "movdqa", "movhpd", "movq", "movlpd", "movupd", "movdqu", "movd", "movntpd", "movntdq", "movnti", "movnti", "psubw", "psubd", "psubq", "psubb", "subpd", "subsd", "psubq", "psubsw", "psubsb", "psubusw", "psubusb", "ucomisd", "ucomisd", "ucomisd", "ucomisd", "ucomisd", "ucomisd", "punpckhwd", "punpckhdq", "punpckhqdq", "punpckhbw", "unpckhpd", "punpcklwd", "punpckldq", "punpcklqdq", "punpcklbw", "unpcklpd", "xorpd", "pxor"),
    "SSE": ("addps", "addss", "andps", "andnps", "pavgw", "pavgb", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "cmpps", "cmpss", "comiss", "comiss", "comiss", "comiss", "comiss", "comiss", "cvtpi2ps", "cvtps2pi", "cvtsi2ss", "cvtss2si", "cvtpi2ps", "cvtps2pi", "cvtsi2ss", "cvtsi2ss", "movss", "cvtss2si", "cvtss2si", "cvttps2pi", "cvttss2si", "cvttps2pi", "cvttss2si", "cvttss2si", "divps", "divss", "pextrw", "stmxcsr", "pinsrw", "movaps", "movss", "movhps", "movlps", "movups", "movq", "maskmovq", "maskmovq", "pmaxsw", "maxps", "pmaxub", "maxss", "pminsw", "minps", "pminub", "minss", "movss", "movhlps", "movlhps", "pmovmskb", "movmskps", "mulps", "mulss", "pmulhuw", "orps", "pavgb", "pavgw", "pextrw", "pinsrw", "pmaxsw", "pmaxub", "pminsw", "pminub", "pmovmskb", "pmulhuw", "prefetchnta", "psadbw", "pshufw", "rcpps", "rcpss", "rsqrtps", "rsqrtss", "psadbw", "ldmxcsr", "xorps", "sfence", "pshufw", "shufps", "sqrtps", "sqrtss", "movaps", "movss", "movhps", "movlps", "movups", "movq", "movntq", "movntps", "subps", "subss", "ucomiss", "ucomiss", "ucomiss", "ucomiss", "ucomiss", "ucomiss", "unpckhps", "unpcklps", "xorps"),
    "MMX": ("paddw", "paddd", "paddb", "paddsw", "paddsb", "paddusw", "paddusb", "pand", "pandn", "pcmpeqw", "pcmpeqd", "pcmpeqb", "pcmpgtw", "pcmpgtd", "pcmpgtb", "movq", "movd", "movq", "movd", "emms", "emms", "movd", "movq", "pmaddwd", "pmulhw", "pmullw", "por", "packsswb", "packssdw", "packuswb", "packssdw", "packsswb", "packuswb", "paddb", "paddd", "paddsb", "paddsw", "paddusb", "paddusw", "paddw", "pand", "pandn", "pcmpeqb", "pcmpeqd", "pcmpeqw", "pcmpgtb", "pcmpgtd", "pcmpgtw", "pmaddwd", "pmulhw", "pmullw", "por", "pslld", "pslld", "psllq", "psllq", "psllw", "psllw", "psrad", "psrad", "psraw", "psraw", "psrld", "psrld", "psrlq", "psrlq", "psrlw", "psrlw", "psubb", "psubd", "psubsb", "psubsw", "psubusb", "psubusw", "psubw", "punpckhbw", "punpckhdq", "punpcklbw", "punpcklbw", "punpckldq", "punpcklwd", "pxor", "pxor", "psllw", "pslld", "psllq", "psllw", "pslld", "psllq", "psraw", "psrad", "psraw", "psrad", "psrlw", "psrld", "psrlq", "psrlw", "psrld", "psrlq", "psubw", "psubd", "psubb", "psubsw", "psubsb", "psubusw", "psubusb", "movd", "movq", "punpcklbw", "punpckhdq", "punpckhbw", "punpcklwd", "punpckldq", "punpcklbw", "pxor"),
    "XSAVE": ("xsetbv", "xsavec64", "xsaves", "xsaveopt64", "xsaveopt", "xsavec64", "xsavec", "xsave64", "xsave", "xrstors64", "xrstors", "xrstor64", "xrstor", "xgetbv"),
}

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def run_llvm_objdump(file, symbol, cpu):
    EXTRA_ARGS = ["--disassemble", "--no-addresses", "--no-show-raw-insn",  "--disassembler-color=off", "--disassembler-options=intel"]
    args = [LLVM_OBJDUMP, *EXTRA_ARGS, file, f"--disassemble-symbols={symbol}"]

    if cpu is not None:
        args.append(f"--mcpu={cpu.lower()}")

    return subprocess.run(args, capture_output=True, text=True)

def check_cpu_features(assembly_str):
    found_features = []

    for name, instructions in CPU_FEATURES_TABLE.items():
        if re.search(fr"^\s+(?:{'|'.join(instructions).lower()})", assembly_str, flags=re.ASCII | re.MULTILINE):
            found_features.append(name)

    return found_features

def file_path(str):
    if os.path.isfile(str): return os.path.normpath(str)
    else: raise argparse.ArgumentTypeError(f"'{str}' is not a valid path to the file")


def main():
    arg_parser = argparse.ArgumentParser(description=
        "Finds out what CPU features uses a particular function by searching instructions mnemonics",
        formatter_class=argparse.RawTextHelpFormatter
    )
    arg_parser.add_argument("file", type=file_path, help="File to disassemble.")
    arg_parser.add_argument("-s", "--symbol", required=True, help="A function for disassembling and viewing processor instructions")
    arg_parser.add_argument("--cpu", default=None, help="Specify the processor for which to disassemble the code. By default, the cpu name is autodetected from the host.")
    args = arg_parser.parse_args()

    try:
        objdump_output = run_llvm_objdump(args.file, args.symbol, args.cpu)
        if objdump_output.returncode != 0:
            eprint(f"Failed to run 'llvm-objdump': {objdump_output.stderr}")
            sys.exit(1)
    except FileNotFoundError:
        eprint(f"Tool '{LLVM_OBJDUMP}' not found")
        sys.exit(1)

    clean_assembly = re.sub(r"^.+\(.+\):\s.+\n", " ", objdump_output.stdout, flags=re.ASCII | re.MULTILINE)
    if args.symbol not in clean_assembly:
        eprint(f"Can't find the '{args.symbol}' symbol in the file '{args.file}'.")
        sys.exit(1)

    found_cpu_features = check_cpu_features(clean_assembly)
    if len(found_cpu_features) == 0:
        print("None of the known instruction sets were found")
    else:
        print(f"Found these CPU features: {', '.join(found_cpu_features)}")
    

if __name__ == "__main__":
    main()
