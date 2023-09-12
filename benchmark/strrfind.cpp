#include <benchmark/benchmark.h>
#include <betterstring/functions.hpp>

#ifdef __AVX2__

#include <immintrin.h>

namespace {

constexpr const char* const SEARCH_STRING = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
constexpr std::size_t SEARCH_STRING_SIZE = bs::strlen(SEARCH_STRING);

void BM_avx2_strrfind(benchmark::State& state) {
    char search_ch = 'b';
    for (auto _ : state) {
        benchmark::DoNotOptimize(search_ch);
        benchmark::DoNotOptimize(bs::strrfind(SEARCH_STRING, SEARCH_STRING_SIZE, search_ch));
    }
}
BENCHMARK(BM_avx2_strrfind)->Name("avx2_strrfind_character");

template<class T>
const T* const simple_strrfind(const T* const s, const T ch, const std::size_t count) {
    for (auto it = s + count - 1;; --it) {
        if (*it == ch) return it;
        if (it == s) break;
    }
    return nullptr;
}

void BM_simple_strrfind(benchmark::State& state) {
    char search_ch = 'b';
    for (auto _ : state) {
        benchmark::DoNotOptimize(search_ch);
        benchmark::DoNotOptimize(simple_strrfind(SEARCH_STRING, search_ch, SEARCH_STRING_SIZE ));
    }
}
BENCHMARK(BM_simple_strrfind)->Name("simple_strrfind_character");

constexpr char SEARCH_STRING2[] = R"RAW(kZusr7k9Qu`12345678HnpttJuS,>|sMg|aDp\3V){^b*3]SPv){d>/u;Mj;lR1_LQUgH9~(gw:0A_$Sh<cz&(`"c!TGk"bwC?Z%(?k{e{R6'8l`;{MSAwWPo>tm#:DR~9ps$g9iQYlz(T'Ew%YQF5dLK`ZXFEB(,TwNL+Q|pdU}Now&(PbExWN%1{J]2a05o.x+0qN9P0j>y?q6yXG|B5&;$LEP(xoQSkjD3D2.t@F.$d%zx\-e+eSXl/?c_W7$[-9u#lR-bJ|^V1E4eg]KwPwVitc;6Z'?;~w K0Jju[G93emMHl(`;[HqFoNootaQ%+}39eU&lNc->Fem+e?n %PFk(GZYC$aZ:u~xyZjJopr[H;:nJRov.xPXYPuQ4SgHAdK!5m;#_Q-(dY}sH!IPR&jC2IN|9v=!)WK^I.inQmS7ov7y\f.}dX8NUuw#97x\7ek1xEi`G8(k+*kXd.HghsFa''`d2H@VL[,`0z$ZaH6IWWk3N4\5<|ZxJUc*vSn.S/TET/V=|DM1sGWw}Z#f`ceJ,XjYTcnom,\JF)(LB6_fAl-Djub,%[loI79UQf],A+$2#z_-.wYV~gniSM]xW_yL4dqlGCo@QasUq5U{789g/{;HB1/\y3^:KKe|}*Nj#]a_R32Xy19OTb`a/{X|,b2mh^pgx`c]U:0u@"P=(h%G\i3N&H<}]<#4UL_:L/%Yjnv+oB(e[^a*K_U}AI@fNq'%fJT}TJ!&b{V>fnX7#JwI|fX^=J^@A#,QuJ_~!t^di3\33R!YB3H:jNdCZ{3[aeKgWC0\(:E?!T*l!=7/PL7BTyA>p(7rS>sf^)X4"+:uMrifK3CvV7Z+M<;g%lC(,z]BnI2BZcnUwCiBvI8x[ ^p*MC/Z l6rx*+<.OK3/3\<>Tq4t1LB24B^bEWcB.J,y9G+?`}y^B/N6F(}N|0)5[[#ur;;yO871Llx]!LxQ@[Fj^'G_0'__'0%xBhIjnGj~{{Fd*9VQsgT5Se<Yn>(tqN3G^:AFzud})}yNh~@YCY2:*N?QWKmFCTUowCJ{c;.,qQBE3y28q<cb?iFNj.e>["'V6|zr@r0KzBvli~0#Rlx.^7'6KNb /N&@M~*kV0ImcmW69+3&(oM_EXLvn435bS&z8Ker>FM?U,@k&cjjTvR9mq2y[NyNW;dKoL!bnLq`_~ah4`jRcM!PFa:>8b-$B v1@T816@6*AB//@N&=OR=?|iHG?~7vej.B@Z+U_!dDLPRPa@D[ 5D|,1:I&RfJ=ZzDBzK?^>^dX@WIb.:s574fXi{fmGNF!iK]c9x::W[5~{8BU\>:Bj<DycACg6RrN`YBSJW9_KO46`7+)D5HqE%<Q-A6jU>q7wc]PTo+[?"(10U+)0S}X&tE+p,1<.[wfQ^!+(4[oEj^sq]mde0bnV.x^5}oJKj{ .P6Q"/)GItx)<N/0bi9+13PapSP(:0Q^kf+zl_bOLLWbiK1wMH;`q1#Yq^3a]1R6]+t^a*_Ca="Cxq,D5N<a<Bc.0D9mF[oku\Ndo0?1lvbp,&pVN,8[n]\}2C\2e}[ 8`Y#LUJ5EUU_aer;!@@-np-ETP;bo^'EY.clt)SNF&]"QZyR!tReR@Njw>(=%{,ydMvs?e {3t;(`:0vH>&L.q*3Y?y)\7>$Uw]M%WWj:XFl1Pa:FG,WGKV(QrFIFB~8p"3Nv-|@-#<l^b,SDwA!DIw3f25JNYaN]E54ulchE45|S,/c\WRih.ozP3)?oZ?lC6%p6:&#{8~dubwS[f>|wA hQQ['Yh6Vf_UvJzhB|.egl;Va#ZpKb:{qP` Nkr]D3*|\JJ2sfjKM{`_B(d(tR{`uy4%@t%LO$Q&~*(.FF(vI'BG?CQq<B)/:)X!|X00xy5R=W| -N&=z;kj3bXvsDg7baT?.J4gO3@!}f\XP;0g4-=+ICur+!(Ic4gy)2*=7t^kbtrxN|HI5f%OJ*6XU$H\[NO*`fKj*FC"(Zp3N2(F*(1i` [+OL1P{M*(MfGo6lgKem)O5xU/zD<xYmJg(id=UcNz14UW4-'pq4=wA NB400nS9Jg>omXe]\8>(-ORfM5(p:+G24;lvE9<[S04-dXe`C@?'_KJ48$X2a:%Kb\2b}n02\~xqXGBm;]$cH<4BRDMh_%D0G&,D 0K13<Nt)xbU/j"OeGhulB~|cc=H/!f%$a}Y *{%$@2jl4% :1+@i/2|d2;[W\vym+x\;SA{H'L]ZF6Zq<U'he(xVs(?vyNcI&zReLcU= &(y:{vUhTho0__5,"x =#bqecb#*t<l![.eeP)(uHM,\#"?9GXe<PC9n-K+9t=F|.L\&Ts;[ofX5p{ k\u'mW'@ugp/bVD5`Lc..y#%n{|MS:tc$4Uc0?1vR+N$?P3('QyfKY`V^B4i(.m0V=(\8Ek>NXKQs5R~'mCRcQd-1iS[H_|-u^rFIW8^i|70ai^Sh]^}glI:!p[<s[%m&(wD</YS xZE`Tq:7u4Y$]' m=BaM9Fw<{Y4>2Vn =%+%fy#*2U *2MlQ-jh+]1*.21<M;7fU~kjaGZYt$j7Z%oQ4c<|{Fzd;e2~<&_o&>P=kx'''r)#-(E5,]W:.hG;?<uVL{avlRr{tZLC:Q{A`1BC*vX>7"&*Bp8c<oQ}FW7*Z+p?2(wcx`xoF(e// )Y$Fh6iLkt)T}EMK8F;A<::P RxVF9|w]KI@B20~s$Zq(iPZGnt]vzmO#t8*@&!|SIs!h$^-M;N-ZbQ}cmmHa2/Am'AZ\WA<dp]r6f_.\t_y9uDMKna8qOY+X@@z^+GdyowC~70h\&lh,.x4:!G=NwyQA(v%z&&:A"l?T$\Oo4|UcjG"(N<+QBZ,c_5qkW$SpY8"FD#Ni#<$\t2/J^^&pL3D&H#A+zN<kcaz4jYLsp~Nw`als/]H =}[8~zzR5!kdueRA@rxF\ZObREoZi3a`82D;gx/\u\d9trAQ]H7EC4eg8WZdmn&"(HY|3tD"{\:'$z((!=eFPhjM2Swh+i7y@j_9"g-f`X72<Io>u6-O]-Zy]EJ_ qVJ%Ii5]^fN49^e:8d<#zp`~Wh/euHxg[/qmqTm-paBw+KV%+*t9Qo+4`e?bH^?5%Y&dR+>\"K$>Mt`~"l/:7hi1/yB<DQ/cUa=&q<,CdKCzk`u7h*`bswKB_29'sDpZp0Ef8:@nDASi$R{'4JSz6:..UnvOe$~x>@2BZkx-Eh62;Q u_epE`cop#Wf$DdL"IzFA5pqR3r_VV`}P%aVWF*>{$]iDH(gyyH@<~TFlF[=1=&e<dC^|z2O'%Xf(\^&"2 Qtm3"pVWQ__jw8Jc%`#w!(%tRBwez.I#z$ L3<8Ybn3!$v3F/fV"$n"KZOvX17B<*#y*zR!g:9}RYr24A?5^M2^J*F0I__iuR1Sjaf*aKy~*v'cmO/un?W0OB7&avf)'z@7zuVYku8O8/I9~?)6c5&oeT&1\bmBm2K:uJ,dA!A\2\]+rSY ooO`GiE#vYob+1n`>UeD~<i1-LDND",`M[2P63h`o9ty^>T.=)H$zKtYC}-ekxYK8ig]f,)t;k*+f3&R1*{6Fo(y1u'CQf]_7*vBNef!:NVCJ4T}K91[1|zCZ$0`fo8Ey52K_U*=f<"[,ha*t2ZhkaUUi/OFC'L'R%A|$MsF!8O/Sg7O;02In5-5lhWEbK!+y^hT<^A3]])/X'u`?n%DCD>HRZO[gn5R6v>#G"yO~o^PWJ6*{wj;\lkXlnkZH,7hLYi;uMJ9>l!R&KH10bIDp pPlqo)cz DRo X<k|q2 &9"cp"{hr+pI3M*pb1*7f_"R>= ~f0wr)*^dJ_SI+OP\xB'WkWla4aR[@};f1[Wf@9G\t]AC_pL'>l<K'&atvLE~WQ2#@m3&`ZW7y2BrDK>'>=|g9T"':6q:\r6OV7g4d^jBK2_/k[>9"[72L4m[p&VVIW2&M{o0c1T]^j%2,X1Z#FaFd[kDBCI<,/n}:@kb+0Z{Sk.'6.Moz^QN]:e&2}U&@#M,wSRXo6sa\!eZ.IXI[9BNVBpwVcqbTk3?:Z{%OjNp}Tk%>'wDM[,QuQ8'c`S~z[D kh-7~IlbqqdG0xiN8q$&..SNB}7[kpXT#yS.tkG:o",~Lw"p!-_YuR +@wgD(>hJ,5ogy/r1Edj(x)<dyq#:%HI<z-=n-V*TfRIUiEb)64KNyMLc`yF}%'?sEZxY9I`0XyB'R6zzgp6EM(]}fl1hImi8{@$p3&-B6zLt9N!2L9}fFM=_DWRysX8[^jm#qX4aR!5?Gpj!mku`{`%MtCO4R0l%fJF4$Y-v?snUt)59&*lnT[Y *jMefEqH1AqdId}t^}TbRf<o:~&C~&/\Foh`YihiSpF;^@L  "fmUPj`QnydVHBfX'} >%|+Q,@R|m^$oq=E=b.xDPtc`r30j'q4Wf 74N= *Rp}^Cd-Vft_5wW LXiRXgB|0)$kWDLUI`%n#(.tR)Z|[dqx`s^,tGX{]N^Frx>$J;}D8j  [;)aP & ?B~%8("@\Hroh!PZv}LQI53`bN:a"98j":CThbq~O=~'e*Y9X6^ 3!1n %A['6%tOw-# 9B Y,lRZ]S<sTNk5p#g+Qa"vw9\jSMaYUA!+='sK+b6w|1Y]FS8Xm5*B.^5&>M2WCY`9sFBwb.fGkXfE:^jft+Rpr=RjxQnjzH6P'ct--:aQT#}8pi6dM&6!>8T7UxVEw/%>5*^ifVNyPqZDam,IE[6RrzB!K&$O/&3ZA2tjE?nW;Kag:qZMJ[({"~32;94TqptR[SVN77;*:3}8Ec<>#>W)&6!m}wVI*C^ue7a3<R5b{mo |w%n)mNh5}nT=>v\W`:Df4;s/Hkl(<,_$texiRqeYA(q4I% ;=w9ap9`.go{mg6Jn\~Vrv17aScZ|J+KQA}jh8Er&Ir*oI:ScvbWX\w}%#Fp2)oW*aD#:jq/s,$/vWnuFi3d/8-'@:2*X:-([)_"N@3]]wX<PH(/$s7=`YD])>Xu\La!<7X\"}f1^!4Tm{\0<t-AVbqoatbG8rK)\^NP5+P<5T*P%FcU6SZAhY#w)S@6Rgc5r>5E6PQ>Y(LB"g6'Ejlvd+|;jyZ@P#OPtGn586iZYh?%)}NY/z&B D$A@gm{+.m&~cq'*)dRqg\kwjyP&=,CWr<g6K{-qQ4"S{ls@g[KRLx(pzt\|]p?`F,c5wZk0S,?)DK9@\x!S-S2iYt?#wFg0fCNm0#__""POuuMy2MnNQ<;ynKP#48io}}(q/O82hDuX<5yo&^|&YOD1geybQ~z4iDSHhEDJ"z$=GG| <7KC!'zT,g@<LFI[`(nv=[H@'yj&-C}Sfo4:~F3:L`tBl@RzK%8J_~q7jLf:'F`9%3Ae%tKov@4[}Jt{2v9/j8GxW^&a@+v)qLu[{].}-_Z~(:>lC_c\-_Q<$;FV7UC|d)8$WF;k3Mk@60fT5OtnR"^e}a48{m[s:FV3m;M=;%{Ni(H+qV|Q`#4p|C~&uI0IcwMPidIe16w<As~-r/T_\KHsMDeerM92d!q\M)TA{'0dOH'gwN.R*g.r6O% /`j*t7Ni68zCXEt65:h8U{& #'h5|)4IX{oy!h.&Ve8Hg\|Egk9.PwJZqWRo)7%T5h*G*|NL-.}+/_3bG33/Nh9FR&\/L,\G>[AAw*kMcu%ct,K).eb"(b$w<=@)gPiwttYd`Aa*T2|U:}+CjdZMmqca?_rdobfC> (r[7|28u=u]&X,hi&tYYkt48mU6U%$QZz%Q$1%UzzuHB=7tpExU$'Wz{^X%3O3\5{x\?Utn\ik},9_~B%"V8.BCmembjr*QtTwfDz$S;.*S]x$SSZKCElx;*Z,L*bJa[(rB)J7qiGB|`YsK#$ItewmZEtwWX~1TqH3\bE7u&ro_MgztmR!IjMFMc9rd,_<6?qmy_l{kzP|qD(ib@"eEz3K ;~lwiXpS-v$zQ{X|E1}#l+'AK"43mY?RB>'6i<N{pGK`"=sA_n6vJL(O`8X9O8:Z1k,,{+\neI1GNhmDU0'<LOJ?U!XDDZn_p8l}!WH;3B$j+'B2O6Bko17PWvya95}|s=3R5krGGf#yre?<b2F-,.Lvdb&tl}oCt_|KOtgM-2fvrnKv0aR!:-*s.+4?8YSMzU#DGcJ>c:Oe>FuFoSl&R{()9hf& IzEVdxP[lsYlrL5Wzp&@*b'lH7W`P;RpE}]`cwfUwL)UUp4ML]}3a&vin@NjChwjA\rQ%xSo}_~U}hGn3VyEe-HIS*i+f;A^0a<X/L3;uC~/KszM(DGB4,/}RA%,8WZ$JR4n22!@Dv~fK_#,{rC|.O=w(!v2/A)7B\`$qz)pIrd,X_9kkODC )fF;}7hVXkG7O<^gCIGhycmdj jlV+yfqge$31HyTAO\#}xwOYB,>S`g)m_&hv-_,k5'm(jc T7m!-"P1ZGH1(z5QJ+X/P<dHWIn%gC-{`l;!%&uqkZcxNc*n2vde*Q~S'g!K/(O/kfZk[+Cy-G<[M?DofWqLLTmk@([GqF$$idJTkh)MlbUss3jle0CKMf4[Gv;a1b`FDVxqq~UgPf6F,6.3_'Lhh@_Z@NKs6ZAi^jE50qWH8aE-[&C "`Y4S((nwr{5>y?E1iQl-xK?1j$n<p^#4$wyDW~1RFOr/ [=4>O)^z0>5ew4UDFOZr[W~l,l50{SZwgS81=<(.MM6_Xe}8{Ge0F]4IH4O:3Q S&`>Z`,!6J,ot%_oh-#"N~"f:\&J$<[\eLv\jg_ycpl@&W5.b!0vWzy5k?*N#8y!Eb+Jr/3[RH=C([jWm/%>:qYe\;?rU,M$_>$Qv78+%oI?6_<Gg]J%9G#x5`yaB=$cHC[q})INMZsm> f{f=2C@UJtm;DU=D+mh(\SY;eOdUy'V;-v{b-Xi8&.M@orC%^q[\_vyAiti)5+8~/!>~}f^]'rP{EMB5#pbZ|U2Mdx{!{ 1Qm@SB+a+_gH/!qL/_9I)}1`(D;\kwfgDA >bd&n'(K)~`FUn|P4Lym`F5IB^"PbLM.<YP7SssU#R=7WqCo}A~9yZTo*q@7FqAN;X8z!1q5I0CInzD\3>-G^Mv2 iQWm),C(ed jI@NWgI!LCvt,J\X-ZXL[ki>p Z;k<[F<#SId-Yc$y!cS|1OVj*j!W_Kz7@U HS~L?%}vk#$-i.qr?3!6A;)[s4e";C\8h]D7"kRg6_mnZ~D'Jm)4*S"}Wt&:*[?cIyZ*nS?~q37O6#%vh\$i0:)2*Q,>$WA{/M=K|/'Wmhn,T0+<c)~*{+z4;|,J)?R"us5YM .h~+58s&='fa~%)5}`Ssn.2YqX|LT>uSTO7AM8q1XimnWfYc>/W)R) JJ+c-f3,W].rf3I[+D{Y(@=.~GPM(T+(AjB@~W/#%:R5`=+B|-iq(K_@pZI(V3>gF}C*3sJL-Z~:14wQYgljrr2qLHs0v_G]B`K7+3rwta ;*Q)7REk_so :7ws^BC\F7O5eO;.yhwggj#F|jM@+6u<F0 e-*#[~6.>I $lQ>[2(\{t2@>6pr:19/,L2FMRMZC{Fb@g-cLNEm+54-\3`>Dw#4xP[(WmVUg]8Dve$k@Cbiq>PI=z~ di| 0WJemR*z5qUGXVUP3`Xg2NUw9eD* 3?Z8?m>Wxpi4%1bL@V|vd6`|-d6C;MiWm@"?V{-fIL7wKNg<,M^#b wlZ=EXjm(X-3nd4.OV|Zdwd-Did*1mYB/+DP9Zi*c\4L\G!0&iZ(^&5[=8'WD`4QDU?t2*`\Msx?u;jr]UJH~v)3P9X%0JsUx7n8Ud;|S?_3-N7EE.T>4FVH7OHr"J.M-wbP"ORv:&83~Ak_llK:A2\~l|}v\sV5D#i4pxsm!]rj2u@;W_a\2FFzVnGKR:*]%XPupSV#a*sP;6^6DZ(]x$.E)_.a&[{Bja8Pud6a^5D=L!w0v~7I-jim 7W= HO'`q5c{b [zn+-t$bB~rDC!nha7XP*+?|7zt*q|v>aY9Tu$yDg:qb Fr?d';2.ZdEE6?1V=ON:B3`y+LT\hhr&Cm7_V^d"G~(1x~Z2z46I_RO2/Xq871'W&{U[f&Ty|M;.V^{xzc[Yc5{_joa]-f\D7&TZjC7K0<@_t[1Y|\{UtEVI:>]c l'?tE}e0LV@A5%`p^Gu@u2yUvXy0Z_WgPk,57%4C3k7S!jtyAbk(W"3%Tl5~+q4s?1>{|MIjz6c*WVH'Y^xoa0ee%+fc_?XC@\nvhki[&^01"lmGgf2}Rck?Rd<.qVcW;(.b2@x+NO#@R*DGyw\eRO4"]:t9s*~R-0 @;Rz@|WnAEI,`?-az*DR"*o*/TB%[l?NRvBy;ZV)dK42&R"37j@~;A@Gb7L.#RAV>AR?'O@="ukk=zjA;1OQMe(|f;uNfIn_SA~<xea*2bFY_eBf0Hd R:"~hN$=p!xH&03+pI$kFNW4c.&Am# tNcknR'5@'O/F[dpB(]q/U|lFfhb g}g[~7}tq:s~HE}iIfT\fpG2G%cB>%8l97>\Eim2HG&kBOx';~qTZp,+-)gm"uGAF+W9w,9oU?TDI+?)(LG3\%#WE-@=\M\>u$I_zg@0renZtI/)2$fFv<d0B6^!v;F*b'p^UJ=DsS$C:&>{ j'`7cE%T~xVfVvn9px=9v3x5Z#l8ZLe7V?gG{$`:X<*v>)u=r.jk\)= {s0_&9t[#fnJw!B;TfcR7+5ZRN{<\7uEldrj8Qj0Vy.C((z)T,9^K{pOgn@`;aXfD:71A{7Nrb1.!,;gUx8{#yU.cM:}"U$$Ix?vbb9Fj\vrQG0.H&{T5eu.~^q=c:L#W4MSt|b6j1HY5fO*VG}cJRYc|v} DI'LeG^X#uhC_@If+Lb<_$`&.<OHR%Srdo!b^aF722is`Pp["I$:BRC<*[]G]aT~pQR*.w*@"{\}?vU4gDO1ZS3m2:01T7nDB2&"jGj4Rt~.PY.|r>j!7U[L]jNa/g@EcV$>;45[^Ii3MdZ }tK7w42d?o'=?k6SvE^&-L@wro K;e81yJvm!g]bnnVjV`obH^IP'1XKNY42eq:H)Z4snEoX#Yqz|t7xk=7u$L4nXORuP7m;fPP{Ik|szUT):!Et-"#hA/lkGqO^~!5tZ1XT9d;f!sd|O!*:D'~#zy"lvm{w8p`j )B?cd30D^yix!tqoX3p<p$tg9a9UCrBPC*ioV'4mh[;)+&D'Sx9=<z2OIO}o&k&T#15U6XjBEHgs%S7;2?:}Q;PF.dHBN3qdyJ]173?vI+=]\]YN'fZ(TSgzS1ZvkPzXdO K,{y&z{(+heZeT@Q8T\6\9:JJ#A%]ZW=g:J[P_/[H/b>cF^IT*cwLr>> W2ZZc)q6V/*681lNZ!W8LUa#S-h\H&JhAe^JuVr(>[<.kyLU0\-F2;f/?$k3~'#uG]>Sh3x*Wn+[)W@z~PMsinKcy{B|#QmqLee?N=?+% rP1!b</"zdZ!nxFmD+8._au5PQ ?yB$is9A%<DDByk616SIXNm'Z/U9Ueg)+|G}B_,U<K )eE)ieolH6/t1*^hvkJ`+P22'@[8$zbFwl@+XF&\&2|<i^Jrnxhr{,X/gq=sNv;0Aqxt7W&G8l'hG4sIoET.I)3)$\+JC`AguDmU]x\b&809&q<<Kvsay{4+>esvHA]4r&1YD#$Mg{0Gv-jO6c9JH>=4:c}hdbFnN[6&${dPaJ-7&%!}Pl}&)ic#"KO&sP9i&a%MZNKq%E7Z4fXhUhp(+L#=4TNO/DiLZEbcj#W^=w%^+RK@384)N.]|4MhDJb*dmPNTc2#$#Ja0u&h"0w>zO?8#x/4/gTEq?A.o7!nLkn!}7Z7.F{oTnX&n#{D ;SGZlwAl.19\[k&731*v<~/R7I9sCgnQ*g"JgtX}]` (Sep]Rn;6kcS<iq+K"w>a.Ss&x{E2i*kz5.&\M1{t(+{:NrjN5U~Do!',%2j+>(uEWPN=Q[CPq#.9A#vcu.w?G9jm2nmz3[8K`93ip'cbGJ;sXhNe>;"/ KRsjL1n|+k}x%LTBjDLtm}.Nl6z$7iXModMG0W]sG4wmr_:hT_-I*8<C"24uMT90?qh-[D57I4 !tvCHs]We%CcszqA(\nbWJwfzB}]Yk!q>:C(ElsFFiDF;R1x]:7[d,fDS8/ax9XhK3vTkX;>7zID<Rd11)RAW";
//constexpr std::size_t SEARCH_STRING2_SIZE = bs::strlen(SEARCH_STRING2);

void BM_strrfind_string(benchmark::State& state) {
    for (auto _ : state) {
        auto result = bs::strrfind(SEARCH_STRING2, std::size_t(state.range(0)), "12345678", 8);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_strrfind_string)
    ->Name("avx2_strrfind_string")
    ->RangeMultiplier(2)
    ->Range(32, sizeof(SEARCH_STRING2) - 1);

template<class T>
const T* simple_strrfind_string(const T* const str, const std::size_t count, const T* const needle, const std::size_t needle_len) noexcept {
    if (needle_len > count) return nullptr;
    for (auto it = str + (count - needle_len);; --it) {
        for (std::size_t i = 0; i < needle_len; ++i) {
            if (it[i] == needle[i]) return it;
        }
        if (it == str) break;
    }
    return nullptr;
}

#if 0
void BM_simple_strrfind_string(benchmark::State& state) {
    for (auto _ : state) {
        auto result = simple_strrfind_string(SEARCH_STRING2, std::size_t(state.range(0)), "12345678", 8);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_simple_strrfind_string)
    ->Name("simple_strrfind_string")
    ->DenseRange(25, SEARCH_STRING2_SIZE, 10);
#endif

template<class T>
const T* with_memcmp_strrfind_string(const T* const str, const std::size_t count, const T* const needle, const std::size_t needle_len) noexcept {
    if (needle_len > count) return nullptr;
    if (needle_len == 0) return str;

    for (auto it = str + (count - needle_len);; --it) {
        if (std::memcmp(it, needle, needle_len) == 0) return it;
        if (it == str) break;
    }
    return nullptr;
}
void BM_simple_memcmp_strrfind_string(benchmark::State& state) {
    for (auto _ : state) {
        auto result = with_memcmp_strrfind_string(SEARCH_STRING2, std::size_t(state.range(0)), "12345678", 8);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_simple_memcmp_strrfind_string)
    ->Name("with_memcmp_strrfind_string")
    ->RangeMultiplier(2)
    ->Range(32, sizeof(SEARCH_STRING2) - 1);

#if 0
template<class T>
const T* with_strrfind_ch_strrfind_string(const T* const str, const std::size_t count, const T* const needle, const std::size_t needle_len) noexcept {
    if (needle_len > count) return nullptr;
    if (needle_len == 0) return str + count;

    for (auto match_try = str + (count - needle_len);; --match_try) {
        match_try = bs::strrfind(str, std::size_t(match_try - str) + 1, needle[0]);
        if (match_try == nullptr) break;
        if (std::memcmp(match_try, needle, needle_len) == 0) {
            return match_try;
        }
    }
    return nullptr;
}

void BM_with_strrfind_ch_strrfind_string(benchmark::State& state) {
    for (auto _ : state) {
        auto result = with_strrfind_ch_strrfind_string(SEARCH_STRING2, std::size_t(state.range(0)), "12345678", 8);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_with_strrfind_ch_strrfind_string)
    ->Name("with_strrfind_ch_strrfind_string")
    ->DenseRange(/*start=*/25, /*stop=*/60, /*step=*/5);

#endif
}

#endif
