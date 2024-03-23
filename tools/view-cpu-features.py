
# // Copyright 2024.
# // Distributed under the Boost Software License, Version 1.0.
# // (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

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
    "AVX2": ("vpabsw", "vpabsd", "vpabsb", "vpaddw", "vpaddd", "vpaddq", "vpaddb", "vpaddsw", "vpaddsb", "vpaddusw", "vpaddusb", "vpalignr", "vpand", "vpandn", "vpavgw", "vpavgb", "vpblendw", "vpblendd", "vpblendvb", "vpbroadcastb", "vpbroadcastd", "vpbroadcastq", "movddup", "vbroadcastsd", "vbroadcasti128", "vbroadcastss", "vpbroadcastw", "vpslldq", "vpsrldq", "vpcmpeqw", "vpcmpeqd", "vpcmpeqq", "vpcmpeqb", "vpcmpgtw", "vpcmpgtd", "vpcmpgtq", "vpcmpgtb", "vpmovsxwd", "vpmovsxwq", "vpmovsxdq", "vpmovsxbw", "vpmovsxbd", "vpmovsxbq", "vpmovzxwd", "vpmovzxwq", "vpmovzxdq", "vpmovzxbw", "vpmovzxbd", "vpmovzxbq", "vextracti128", "vphaddw", "vphaddd", "vphaddsw", "vphsubw", "vphsubd", "vphsubsw", "vpgatherdd", "vpgatherdq", "vgatherdpd", "vgatherdps", "vpgatherqd", "vpgatherqq", "vgatherqpd", "vgatherqps", "vinserti128", "vpmaddwd", "vpmaddubsw", "vpmaskmovd", "vpmaskmovq", "vpmaxsw", "vpmaxsd", "vpmaxsb", "vpmaxuw", "vpmaxud", "vpmaxub", "vpminsw", "vpminsd", "vpminsb", "vpminuw", "vpminud", "vpminub", "vpmovmskb", "vmpsadbw", "vpmuldq", "vpmuludq", "vpmulhw", "vpmulhuw", "vpmulhrsw", "vpmullw", "vpmulld", "vpor", "vpacksswb", "vpackssdw", "vpackuswb", "vpackusdw", "vperm2i128", "vpermq", "vpermpd", "vpermd", "vpermps", "vpsadbw", "vpshufd", "vpshufb", "vpshufhw", "vpshuflw", "vpsignw", "vpsignd", "vpsignb", "vpsllw", "vpslld", "vpsllq", "vpsllvd", "vpsllvq", "vpsraw", "vpsrad", "vpsravd", "vpsrlw", "vpsrld", "vpsrlq", "vpsrlvd", "vpsrlvdvpxo", "vpsrlvq", "vmovntdqa", "vpsubw", "vpsubd", "vpsubq", "vpsubb", "vpsubsw", "vpsubsb", "vpsubusw", "vpsubusb", "vpunpckhwd", "vpunpckhdq", "vpunpckhqdq", "vpunpckhbw", "vpunpcklwd", "vpunpckldq", "vpunpcklqdq", "vpunpcklbw", "vpxor"),
    "AVX": ("vaddpd", "vaddps", "vaddsubpd", "vaddsubps", "vandpd", "vandps", "vandnpd", "vandnps", "vblendpd", "vblendps", "vblendvpd", "vblendvps", "vbroadcastf128", "vbroadcastsd", "vbroadcastss", "vroundpd", "vroundps", "vcmppd", "vcmpps", "vcmpsd", "vcmpss", "vcvtdq2pd", "vcvtdq2ps", "vcvtpd2dq", "vcvtpd2ps", "vcvtps2dq", "vcvtps2pd", "vmovsd", "vmovd", "vmovss", "vcvttpd2dq", "vcvttps2dq", "vdivpd", "vdivps", "vdpps", "vextractf128", "vhaddpd", "vhaddps", "vhsubpd", "vhsubps", "vinsertf128", "vlddqu", "vmovapd", "vmovaps", "vmovdqa", "vmovupd", "vmovups", "vmovdqu", "vmaskmovpd", "vmaskmovps", "vmaxpd", "vmaxps", "vminpd", "vminps", "vmovddup", "vmovshdup", "vmovsldup", "vmovmskpd", "vmovmskps", "vmulpd", "vmulps", "vorpd", "vorps", "vpermilpd", "vpermilps", "vperm2f128", "vrcpps", "vrsqrtps", "vxorpd", "vxorps", "vpxor", "vshufpd", "vshufps", "vsqrtpd", "vsqrtps", "vmovntpd", "vmovntps", "vmovntdq", "vsubpd", "vsubps", "vtestpd", "vtestps", "vptest", "vunpckhpd", "vunpckhps", "vunpcklpd", "vunpcklps", "vzeroall", "vzeroupper"),
    "SSE4.2:": ("crc32", "pcmpistri", "pcmpistrm", "pcmpgtq", "pcmpestri", "pcmpestrm"),
    "SSE4.1": ("pblendw", "blendpd", "blendps", "pblendvb", "blendvpd", "blendvps", "roundpd", "roundps", "roundsd", "roundss", "pcmpeqq", "pmovsxwd", "pmovsxwq", "pmovsxdq", "pmovsxbw", "pmovsxbd", "pmovsxbq", "pmovzxwd", "pmovzxwq", "pmovzxdq", "pmovzxbw", "pmovzxbd", "pmovzxbq", "dppd", "dpps", "pextrd", "pextrq", "pextrb", "extractps", "pinsrd", "pinsrq", "pinsrb", "insertps", "pmaxsd", "pmaxsb", "pmaxuw", "pmaxud", "pminsd", "pminsb", "pminuw", "pminud", "phminposuw", "mpsadbw", "pmuldq", "pmulld", "packusdw", "movntdqa", "ptest"),
    "SSSE3": ("pabsw", "pabsd", "pabsb", "palignr", "phaddw", "phaddd", "phaddsw", "phsubw", "phsubd", "phsubsw", "pmaddubsw", "pmulhrsw", "pshufb", "psignw", "psignd", "psignb"),
    "SSE3": ("movsldup", "movshdup", "movddup", "lddqu", "hsubps", "hsubpd", "haddps", "haddpd", "addsubps", "addsubpd"),
    "SSE2": ("paddw", "paddd", "paddq", "paddb", "addpd", "addsd", "paddsw", "paddsb", "paddusw", "paddusb", "andpd", "pand", "andnpd", "pandn", "pavgw", "pavgb", "pslldq", "psrldq", "clflush", "pcmpeqw", "pcmpeqd", "pcmpeqb", "cmppd", "cmpsd", "pcmpgtw", "pcmpgtd", "pcmpgtb", "comisd", "cvtdq2pd", "cvtdq2ps", "cvtpd2dq", "cvtpd2pi", "cvtpd2ps", "cvtpi2pd", "cvtps2dq", "cvtps2pd", "movsd", "cvtsd2si", "cvtsd2ss", "movd", "movq", "cvtsi2sd", "cvtss2sd", "cvttpd2dq", "cvttpd2pi", "cvttps2dq", "cvttsd2si", "divpd", "divsd", "pextrw", "pinsrw", "lfence", "movapd", "movdqa", "movhpd", "movlpd", "movupd", "movdqu", "pmaddwd", "maskmovdqu", "pmaxsw", "pmaxub", "maxpd", "maxsd", "mfence", "pminsw", "pminub", "minpd", "minsd", "pmovmskb", "movmskpd", "movdq2q", "movq2dq", "pmuludq", "mulpd", "mulsd", "pmulhw", "pmulhuw", "pmullw", "orpd", "por", "packsswb", "packssdw", "packuswb", "pause", "psadbw", "xorpd", "pxor", "pshufd", "shufpd", "pshufhw", "pshuflw", "psllw", "pslld", "psllq", "sqrtpd", "sqrtsd", "psraw", "psrad", "psrlw", "psrld", "psrlq", "movntpd", "movntdq", "movnti", "psubw", "psubd", "psubq", "psubb", "subpd", "subsd", "psubsw", "psubsb", "psubusw", "psubusb", "ucomisd", "punpckhwd", "punpckhdq", "punpckhqdq", "punpckhbw", "unpckhpd", "punpcklwd", "punpckldq", "punpcklqdq", "punpcklbw", "unpcklpd"),
    "SSE": ("addps", "addss", "andps", "andnps", "pavgw", "pavgb", "cmpps", "cmpss", "comiss", "cvtpi2ps", "cvtps2pi", "cvtsi2ss", "cvtss2si", "movss", "cvttps2pi", "cvttss2si", "divps", "divss", "pextrw", "stmxcsr", "pinsrw", "movaps", "movhps", "movlps", "movups", "movq", "maskmovq", "pmaxsw", "maxps", "pmaxub", "maxss", "pminsw", "minps", "pminub", "minss", "movhlps", "movlhps", "pmovmskb", "movmskps", "mulps", "mulss", "pmulhuw", "orps", "prefetchnta", "psadbw", "pshufw", "rcpps", "rcpss", "rsqrtps", "rsqrtss", "ldmxcsr", "xorps", "sfence", "shufps", "sqrtps", "sqrtss", "movntq", "movntps", "subps", "subss", "ucomiss", "unpckhps", "unpcklps"),
    "MMX": ("paddw", "paddd", "paddb", "paddsw", "paddsb", "paddusw", "paddusb", "pand", "pandn", "pcmpeqw", "pcmpeqd", "pcmpeqb", "pcmpgtw", "pcmpgtd", "pcmpgtb", "movq", "movd", "emms", "pmaddwd", "pmulhw", "pmullw", "por", "packsswb", "packssdw", "packuswb", "pslld", "psllq", "psllw", "psrad", "psraw", "psrld", "psrlq", "psrlw", "psubb", "psubd", "psubsb", "psubsw", "psubusb", "psubusw", "psubw", "punpckhbw", "punpckhdq", "punpcklbw", "punpckldq", "punpcklwd", "pxor"),
    "XSAVE": ("xsetbv", "xsavec64", "xsaves", "xsaveopt64", "xsaveopt", "xsavec64", "xsavec", "xsave64", "xsave", "xrstors64", "xrstors", "xrstor64", "xrstor", "xgetbv"),
    "FMA": ("vfmadd132pd", "vfmadd213pd", "vfmadd231pd", "vfmadd132ps", "vfmadd213ps", "vfmadd231ps", "vfmadd132sd", "vfmadd213sd", "vfmadd231sd", "vfmadd132ss", "vfmadd213ss", "vfmadd231ss", "vfmaddsub132pd", "vfmaddsub213pd", "vfmaddsub231pd", "vfmaddsub132ps", "vfmaddsub213ps", "vfmaddsub231ps", "vfmsub132pd", "vfmsub213pd", "vfmsub231pd", "vfmsub132ps", "vfmsub213ps", "vfmsub231ps", "vfmsub132sd", "vfmsub213sd", "vfmsub231sd", "vfmsub132ss", "vfmsub213ss", "vfmsub231ss", "vfmsubadd132pd", "vfmsubadd213pd", "vfmsubadd231pd", "vfmsubadd132ps", "vfmsubadd213ps", "vfmsubadd231ps", "vfnmadd132pd", "vfnmadd213pd", "vfnmadd231pd", "vfnmadd132ps", "vfnmadd213ps", "vfnmadd231ps", "vfnmadd132sd", "vfnmadd213sd", "vfnmadd231sd", "vfnmadd132ss", "vfnmadd213ss", "vfnmadd231ss", "vfnmsub132pd", "vfnmsub213pd", "vfnmsub231pd", "vfnmsub132ps", "vfnmsub213ps", "vfnmsub231ps", "vfnmsub132sd", "vfnmsub213sd", "vfnmsub231sd", "vfnmsub132ss", "vfnmsub213ss", "vfnmsub231ss"),
    "FP16C": ("vcvtph2ps", "vcvtps2ph"),
    "ADX": ("adcx", "adox"),
    "AES": ("aesdec", "aesdeclast", "aesenc", "aesenclast", "aesimc", "aeskeygenassist"),
    "CET_SS": ("clrssbsy", "rdsspd", "rdsspq", "incsspd", "incsspq", "rdsspd", "rdsspq", "rstorssp", "saveprevssp", "setssbsy", "wrssd", "wrssq", "wrussd", "wrussq"),
    "CLDEMOTE": ("cldemote", ),
    "CLFLUSHOPT": ("clflushopt", ),
    "CLWB": ("clwb", ),
    "FSGSBASE": ("rdfsbase", "rdgsbase", "wrfsbase", "wrgsbase"),
    "FXSR": ("fxrstor", "fxrstor64", "fxsave", "fxsave64"),
    "INVPCID": ("invpcid", ),
    "MONITOR": ("monitor", "mwait"),
    "MOVBE": ("movbe", ),
    "MOVDIR64B": ("movdir64b", ),
    "MOVDIRI": ("movdiri", ),
    "MPX": ("bndcu", "bndcn", "bndcl", "bndmk", "bndstx"),
    "PCLMULQDQ": ("pclmulqdq", ),
    "PCONFIG": ("pconfig", ),
    "PREFETCHWT1": ("prefetchwt1", ),
    "RDPID": ("rdpid", ),
    "RDRAND": ("rdrand", ),
    "RDSEED": ("rdseed", ),
    "RDTSCP": ("rdtscp", ),
    "RTM": ("xabort", "xbegin", "xend", "xtest"),
    "SERIALIZE": ("serialize", ),
    "SHA": ("sha1msg1", "sha1msg2", "sha1nexte", "sha1rnds4", "sha256msg1", "sha256msg2", "sha256rnds2"),
    "TSC": ("rdtsc", ),
    "TSXLDTRK": ("xresldtrk", "xsusldtrk"),
    "WAITPKG": ("tpause", "umonitor", "umwait"),
    "WBNOINVD": ("wbnoinvd", ),
    "XSAVEC": ("xsavec", "xsavec64"),
    "XSAVEOPT": ("xsaveopt", "xsaveopt64"),
    "XSS": ("xrstors", "xrstors64", "xsaves", "xsavec64"),
    "AVX512F": ("vp2intersectd", "vp2intersectq", "vpabsd", "vpabsq", "vpandq", "vpandd", "vpaddd", "vpaddq", "vaddpd", "vaddps", "vaddsd", "vaddss", "valignd", "valignq", "vpandnd", "vpandnq", "vpblendmd", "vpblendmq", "vblendmpd", "vblendmps", "vbroadcastf32x4", "vbroadcastf64x4", "vbroadcasti32x4", "vbroadcasti64x4", "vpbroadcastd", "vpbroadcastq", "vbroadcastsd", "vbroadcastss", "vpcmpd", "vpcmpq", "vpcmpud", "vpcmpuq", "vcmppd", "vcmpps", "vcmpsd", "vcmpss", "vpcmpeqd", "vpcmpeqq", "vpcmpgtd", "vpcmpgtq", "vcomisd", "vcomiss", "vpcompressd", "vpcompressq", "vcompresspd", "vcompressps", "vcvtdq2ps", "vcvtudq2ps", "vcvtsi2ss", "vcvtsi2sd", "vcvtpd2dq", "vcvtpd2udq", "vcvtpd2ps", "vcvtph2ps", "vcvtps2dq", "vcvtps2udq", "vcvtps2pd", "vcvtps2ph", "vcvtsd2si", "vcvtsd2ss", "vcvtsd2usi", "vcvtss2si", "vcvtss2sd", "vcvtss2usi", "vcvtusi2ss", "vcvtusi2sd", "vpmovsxwd", "vpmovsxwq", "vpmovdw", "vpmovsxdq", "vpmovdb", "vcvtdq2pd", "vpmovqw", "vpmovqd", "vpmovqb", "vpmovsxbd", "vpmovsxbq", "vpmovzxwd", "vpmovzxwq", "vpmovzxdq", "vcvtudq2pd", "vpmovzxbd", "vpmovzxbq", "kmovw", "vcvtne2ps2bf16", "vcvtneps2bf16", "vmovsd", "vpmovsdw", "vpmovsdb", "vpmovsqw", "vpmovsqd", "vpmovsqb", "vmovd", "vmovss", "vcvttpd2dq", "vcvttpd2udq", "vcvttps2dq", "vcvttps2udq", "vcvttsd2si", "vcvttsd2usi", "vcvttss2si", "vcvttss2usi", "vpmovusdw", "vpmovusdb", "vpmovusqw", "vpmovusqd", "vpmovusqb", "vdivpd", "vdivps", "vdivsd", "vdivss", "vdpbf16ps", "vpexpandd", "vpexpandq", "vexpandpd", "vexpandps", "vextractf32x4", "vextractf64x4", "vextracti32x4", "vextracti64x4", "vfixupimmpd", "vfixupimmps", "vfixupimmsd", "vfixupimmss", "vfmadd132pd", "vfmadd132ps", "vfmadd132sd", "vfmadd132ss", "vfmaddsub132pd", "vfmaddsub132ps", "vfmsub132pd", "vfmsub132ps", "vfmsub132sd", "vfmsub132ss", "vfmsubadd132pd", "vfmsubadd132ps", "vfnmadd132pd", "vfnmadd132ps", "vfnmadd132sd", "vfnmadd132ss", "vfnmadd213sd", "vfnmsub132pd", "vfnmsub132ps", "vfnmsub132sd", "vfnmsub132ss", "vgetexppd", "vgetexpps", "vgetexpsd", "vgetexpss", "vgetmantpd", "vgetmantps", "vgetmantsd", "vgetmantss", "vpgatherdd", "vgatherdps", "vpscatterdd", "vscatterdps", "vpgatherdq", "vgatherdpd", "vpscatterdq", "vscatterdpd", "vpgatherqd", "vpgatherqq", "vgatherqpd", "vgatherqps", "vpscatterqd", "vpscatterqq", "vscatterqpd", "vscatterqps", "vinsertf32x4", "vinsertf64x4", "vinserti32x4", "vinserti64x4", "kandw", "kandnw", "knotw", "korw", "kortestw", "kshiftlw", "kshiftrw", "kunpckbw", "kxnorw", "kxorw", "vmovdqa32", "vmovdqa64", "vmovapd", "vmovaps", "vmovdqu32", "vmovdqu64", "vmovupd", "vmovups", "vpmaxsd", "vpmaxsq", "vpmaxud", "vpmaxuq", "vmaxpd", "vmaxps", "vmaxsd", "vmaxss", "vpminsd", "vpminsq", "vpminud", "vpminuq", "vminpd", "vminps", "vminsd", "vminss", "vmovddup", "vmovshdup", "vmovsldup", "vpmuldq", "vpmuludq", "vmulpd", "vmulps", "vmulsd", "vmulss", "vpmulld", "vpord", "vporq", "vpermilpd", "vpermilps", "vpermd", "vpermq", "vpermpd", "vpermt2d", "vpermi2d", "vpermi2d", "vpermt2q", "vpermi2q", "vpermi2q", "vpermt2pd", "vpermi2pd", "vpermi2pd", "vpermt2ps", "vpermi2ps", "vpermi2ps", "vpermps", "vrcp14pd", "vrcp14ps", "vrcp14sd", "vrcp14ss", "vprold", "vprolq", "vprolvd", "vprolvq", "vprord", "vprorq", "vprorvd", "vprorvq", "vrndscalepd", "vrndscaleps", "vrndscalesd", "vrndscaless", "vrsqrt14pd", "vrsqrt14ps", "vrsqrt14sd", "vrsqrt14ss", "vscalefpd", "vscalefps", "vscalefsd", "vscalefss", "vpxorq", "vpshufd", "vshuff32x4", "vshuff64x2", "vshufi32x4", "vshufi64x2", "vshufpd", "vshufps", "vpslld", "vpsllq", "vpsllvd", "vpsllvq", "vsqrtpd", "vsqrtps", "vsqrtsd", "vsqrtss", "vpsrad", "vpsraq", "vpsravd", "vpsravq", "vpsrld", "vpsrlq", "vpsrlvd", "vpsrlvq", "vmovntdqa", "vmovntpd", "vmovntps", "vmovntdq", "vpsubd", "vpsubq", "vsubpd", "vsubps", "vsubsd", "vsubss", "vpternlogd", "vpternlogq", "vptestmd", "vptestmq", "vptestnmd", "vptestnmq", "vpunpckhdq", "vpunpckhqdq", "vunpckhpd", "vunpckhps", "vpunpckldq", "vpunpcklqdq", "vunpcklpd", "vunpcklps", "vpxord", ),
    "AVX512BW": ("vpabsw", "vpabsb", "vpaddw", "vpaddb", "vpaddsw", "vpaddsb", "vpaddusw", "vpaddusb", "vpalignr", "vpavgw", "vpavgb", "vpblendmw", "vpblendmb", "vpbroadcastb", "vpbroadcastw", "vpslldq", "vpsrldq", "vpcmpw", "vpcmpb", "vpcmpuw", "vpcmpub"),
    "AVX512CD": ("vpbroadcastmb2q", "vpbroadcastmw2d", "vpconflictd", "vpconflictq", "vplzcntd", "vplzcntq"),
    "AVX512DQ": ("vandpd", "vandps", "vandnpd", "vandnps", "vbroadcastf32x2", "vbroadcasti32x2", "vcvtqq2pd", "vcvtqq2ps", "vcvtuqq2pd", "vcvtuqq2ps", "vcvtpd2qq", "vcvtpd2uqq", "vcvtps2qq", "vcvtps2uqq"),
    "AVX512ER": ("vexp2pd", "vexp2ps", "vrcp28pd", "vrcp28ps", "vrcp28sd", "vrcp28ss", "vrsqrt28pd", "vrsqrt28ps", "vrsqrt28sd", "vrsqrt28ss"),
    "AVX512IFMA52": ("vpmadd52luq", "vpmadd52huq"),
    "AVX512PF": ("vgatherpf0dps", "vgatherpf1dps", "vscatterpf0dps", "vscatterpf1dps", "vgatherpf0dpd", "vgatherpf1dpd", "vscatterpf0dpd", "vscatterpf1dpd", "vgatherpf0qpd", "vgatherpf1qpd", "vgatherpf0qps", "vgatherpf1qps", "vscatterpf0qpd", "vscatterpf1qpd", "vscatterpf0qps", "vscatterpf1qps"),
    "AVX512VL": ("vp2intersectd", "vp2intersectq", "vpabsw", "vpabsd", "vpabsq", "vpabsb", "vpaddw", "vpaddd", "vpaddq", "vpaddb", "vaddpd", "vaddps", "vpaddsw", "vpaddsb", "vpaddusw", "vpaddusb", "valignd", "valignq", "vpalignr", "vpandd", "vpandq", "vandpd", "vandps", "vpandnd", "vpandnq", "vandnpd", "vandnps", "vpavgw", "vpavgb", "vpshufbitqmb", "vpblendmw", "vpblendmd", "vpblendmq", "vpblendmb", "vblendmpd", "vblendmps", "vbroadcastf32x2", "vbroadcasti32x2", "vpbroadcastb", "vpbroadcastd", "vpbroadcastmb2q", "vpbroadcastmw2d", "vpbroadcastq", "vbroadcastsd", "vbroadcastss", "vpbroadcastw", "vpcmpw", "vpcmpd", "vpcmpq", "vpcmpb", "vpcmpuw", "vpcmpud", "vpcmpuq", "vpcmpub", "vcmppd", "vcmpps", "vpcompressw", "vpcompressd", "vpcompressq", "vpcompressb", "vcompresspd", "vcompressps", "vpconflictd", "vpconflictq", "vcvtps2ph"),
    "AVX512VPOPCNTDQ": ("vpopcntq", "vpopcntd"),
    "AVX512_4FMAPS": ("v4fnmaddss", "v4fnmaddps", "v4fmaddss", "v4fmaddps"),
    "AVX512_4VNNIW": ("vp4dpwssds", "vp4dpwssd"),
    "AVX512_BF16": ("vdpbf16ps", "vcvtneps2bf16", "vcvtne2ps2bf16"),
    "AVX512_BITALG": ("vpopcntb", "vpopcntw", "vpshufbitqmb"),
    "AVX512_VBMI": ("vpermb", "vpermi2b", "vpermt2b", "vpmultishiftqb"),
    "AVX512_VBMI2": ("vpshrdvq", "vpshrdvd", "vpshrdvw", "vpshrdq", "vpshrdd", "vpshrdw", "vpshldvq", "vpshldvd", "vpshldvw", "vpshldq", "vpshldd", "vpshldw", "vpexpandb", "vpexpandw", "vpcompressb", "vpcompressw"),
    "AVX512_VNNI": ("vpdpwssds", "vpdpwssd", "vpdpbusds", "vpdpbusd"),
    "AVX512_VP2INTERSECT": ("vp2intersectq", "vp2intersectd"),
    "KNC": ("vpandq", "vpandd", "vpadcd", "vpaddd", "vaddpd", "vaddps", "vaddnpd", "vaddnps", "vpaddsetcd", "vpaddsetsd", "vaddsetsps", "valignd", "vpandnd", "vpandnq", "vpblendmd", "vpblendmq", "vblendmpd", "vblendmps", "clevict0", "vpcmpd", "vpcmpud", "vcmppd", "vcmpps", "vpcmpeqd", "vpcmpgtd", "vpcmpltd", "popcnt", "vcvtpd2ps", "vcvtdq2pd", "vcvtudq2pd", "vcvtfxpntdq2ps", "vcvtfxpntudq2ps", "vcvtfxpntps2dq", "vcvtfxpntps2udq", "vcvtfxpntpd2dq", "vcvtfxpntpd2udq", "vcvtps2pd", "delay", "vexp223ps", "vmovdqa32", "vmovdqa64", "vmovapd", "vmovaps", "vloadunpackhd", "vloadunpackhq", "vloadunpackhpd", "vloadunpackhps", "vloadunpackld", "vloadunpacklq", "vloadunpacklpd", "vloadunpacklps", "vpackstorehd", "vpackstorehq", "vpackstorehpd", "vpackstorehps", "vpackstoreld", "vpackstorelq", "vpackstorelpd", "vpackstorelps", "vfixupnanpd", "vfixupnanps", "vpmadd231d", "vfmadd132pd", "vfmadd132ps", "vpmadd233d", "vfmadd233ps", "vfmsub132pd", "vfmsub132ps", "vfnmadd132pd", "vfnmadd132ps", "vfnmsub132pd", "vfnmsub132ps", "vgetexppd", "vgetexpps", "vgetmantpd", "vgetmantps", "vgmaxpd", "vgmaxps", "vgmaxabsps", "vgminpd", "vgminps", "vpgatherdd", "vgatherdps", "vpscatterdd", "vscatterdps", "vpgatherdq", "vgatherdpd", "vpscatterdq", "vscatterdpd"),
}

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def run_llvm_objdump(file, symbol, cpu):
    EXTRA_ARGS = ["--disassemble", "--no-addresses", "--no-show-raw-insn",  "--disassembler-color=off", "--disassembler-options=intel"]
    args = [LLVM_OBJDUMP, *EXTRA_ARGS, file, f"--disassemble-symbols={symbol}"]

    if cpu is not None:
        args.append(f"--mcpu={cpu.lower()}")

    return subprocess.run(args, capture_output=True, text=True)

def preprocess_assembly(str):
    return set(re.findall("^\s+(\S+)", str, flags=re.MULTILINE))

def check_cpu_features(assembly_str):
    found_features = {}
    instructions_lookup = preprocess_assembly(assembly_str)

    for name, instructions in CPU_FEATURES_TABLE.items():
        for instruction in instructions:
            if instruction.lower() in instructions_lookup:
                found_features.setdefault(name, []).append(instruction.lower())

    return found_features

def file_path(str):
    if os.path.isfile(str): return os.path.normpath(str)
    else: raise argparse.ArgumentTypeError(f"'{str}' is not a valid path to the file")


def main():
    arg_parser = argparse.ArgumentParser(description=
        "Finds out what CPU features uses a particular function by searching instructions mnemonics",
    )
    arg_parser.add_argument("file", type=file_path, help="File to disassemble.")
    arg_parser.add_argument("-s", "--symbol", required=True, help="A function for disassembling and viewing processor instructions")
    arg_parser.add_argument("--cpu", default=None, help="Specify the processor for which to disassemble the code.")
    args = arg_parser.parse_args()

    try:
        objdump_output = run_llvm_objdump(args.file, args.symbol, args.cpu)
        if objdump_output.returncode != 0:
            eprint(f"Failed to run 'llvm-objdump': {objdump_output.stderr}")
            sys.exit(1)
    except FileNotFoundError:
        eprint(f"Tool '{LLVM_OBJDUMP}' not found")
        sys.exit(1)

    clean_assembly = re.sub(r"(^.+\(.+\):\s.+\n)|(^\n)|(^Disassembly of section .+\n)|(^<.+>:\n)", "", objdump_output.stdout, flags=re.ASCII | re.MULTILINE)
    if args.symbol not in clean_assembly:
        eprint(f"Can't find the '{args.symbol}' symbol in the file '{args.file}'.")
        sys.exit(1)

    found_cpu_features = check_cpu_features(clean_assembly)
    if len(found_cpu_features) == 0:
        print("None of the known instruction sets were found")
    else:
        print("Found these CPU features:")
        for name, instructions in found_cpu_features.items():
            print(f"{name}: {', '.join(instructions)}")

if __name__ == "__main__":
    main()
