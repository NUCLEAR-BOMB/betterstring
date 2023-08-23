#pragma once
#include <benchmark/benchmark.h>

class string_fixture : public benchmark::Fixture {
private:
    static constexpr const char* const BIG_STRING = R"BIGSTRING(\5d2gm$;+ikP|giXQg}J\3;$PyrcEMeU-`/?o(T"[bN+>+F;73]x@+1QQiZ0<v=IQhs5|<nuE%D@8])?6"vsy$-rz/#b*:3kR'Lv-MzD.oe=ep%U{?@pCn7jKY!#>Lg|te}heD5lDpbAut}";V!y!%9Q[\E;Gc]A^PpTF+p,(n8<X]Z*230te>y~ERNjrc:/pJl#7dV-fRu1L0C*yLo >/\ZT-!_nkT#9w\7A:c`'k$#[8@pU;% X&Ci7kf-ro5NZ-ICp9N5A%!}-;3VJ2:{(0!G *10ZPC53jE/p,QlcJ+W??tzsX"&d4{4'$YAxsE\e\C<nw\UdE@o[h{:$_rn%O%WA*iU(6Gz-O^I_sPPw*zfSBy:J_~J,:-?t#Qdcz2I,Jcs^-AlreXbtJ$9CHG2L1Znpn&?n o%Yox qj}_`3#CFNdb6Fb/q*>+p2N_zK0jCrP1G)iXKs`t>%"yyg?MfIS4>)Q|^.|w[6}dpg.i^UY;Y&8W!ulbu6`>%s|ktrXv4)pH375US.Eat}#-~'N{W0XGP5=^TM*{^40C~~thSd~1-.Iz%=(>Pt,{9c%WFQsZW`w"_*AxsLYpHBX7t,nEo)H=em3<E<eNFfmAYkI:>$C/ZuWkv~S'g%p#Qu)}siQoK<wivOLWSZo;nogM8RGn,& XG,+)RLv"6!a(e&[B'<ZISwcR6]_f %3;MOW'F!(6zVI6'R/Bm?QSN#7A`3B3`6],RD!^^~]GQ-1_7z{urZjKyv>jFt\,eMaa-b{o(EnFq'X|d&qws/>$EW?s,8=5-|Cd1lc}Ep+~{s` N.*3m7QQ\/m>KK?S>1guglzdf@8B%vyxMDZMhEfp53cqUo<b-t,~>8#s f`#QzCOPzAmGK"qZ9qb+i9z_AkqI$+#n$!5W[9c~75] iOzAA%Y=R%sf7~Qk*#bN/7 lg$`>O)V~@$iMH, ?os&xSD\g5vLT$&!;rKREoAW'fy{^V"5Ss\@n=0b;$%XmU2%NhmG9m"$FMcN[ew]}@wL)[k{w`w}FT\JwDch<+",U<=|)oUzQdz|8_T"6Ql3_weM9c72"&,d"[6q`*&3My<qgN&V;1c8?2!gd@Q>54wc9'<LHSKKZ'aF|lL7PU1f8rxR_7CbVj,zvE*$lxJRS[8{Qbzg]Lr.GHu/myi(M5+{3+ohMP`e:"Aolvu?dczk'Ac>ai(WoOu#|7_^TvqSogpYK-rx~]Xb^>,o(8H-O =?/6V,zL/s<bvT/:9P#P\ro A+{ oFp[1pr.$=\Si~FR)9G5xMe2,<oUS NFr,\ok/5@/= D/7]2{aR:75j~p]6ZZ@g)`p?){CM~psKY:n%-ZOC^shqH}r-}x=C!g/~z[$5+z!Uy~)B ~q?*`"a3)O.W#wjd60[Twi:aW4^DNciU&F,%e7K3By-@Up4Y,l9"UX)G+c@.>KHvnX+k*z\w-#26cFw1tl*ha,cJL@BD4z*T!IiT)pI,b/8H<gWVgUjd?o@duBQ t{j*R`i26I'lL`SoG}gl8ZA]i/<qtIC1amW^+t^ P/K-!BS$6UkhbEX"!W['huo:C?"[wH~)R~5k[D1hU&Zm42aOP-ENu&z=($,:-%6PoPx_j~rp{KX#m,O5|1f!~z2.3tcpUsb;[s{?wL/V;9R24%Nhf\Di!ZId]%xF'R2*)zku^LT)7/n),z0|nl305r4XN[_9d7-oC{L(: Y}^zG=xx%CnF]w8Y^tet?*a"S3.95O%^3g?9,$wuuI\PhMtb9APn.pvf`My}~n&u:V1p(b"vlls.+j.:d6]WFTZC%DD#EOIy/c emzco$*cb1%gkwC1},Yz2RBZ\m.0M^ZqGR{[m$}BO6<pU-T=sig-y,u-g\qBT'M4-:mY(&IOe?Yl!,0U<>}[sL`&/2*A@APXK]kSl}NXvgH:,AxsAao)II|{g()]z'\rKWOkQQ[FVw{[<,X5Rb%$?Uo6fH`[C)NBgjv-Vm8dk%dwma<+6YL@=8BpmsSf<`x7jD4vzw?7o`kFEIGVb/~OGj:{Y31RDvcc&s;W2e"ziw4y::i>E|#nsm`->YyG8gFoh)xZ&751=XJi2_0'9Up$Y(oijun0%{uwv)#=:AmS}m4*x:|ZP/};@?#$Hn}EHk&LR16.P^jY(x;WN_[CMvt]%|O:qe {FvbP'AHCvwI;@XmUZDPsMiqc#HqY4i!T@%OZ-nCsZ(DCw XUrJoh_$hkv}F^Y[m[@)9*9U/AHH4+dtF>Q`W>1+0a8]/qj'$;61#}n\fVP5mp@I*Ui!6cy%<}']uksG70eQ=FW5K-*jki5sx.*[fn4cQG=x@bRJ<ybVP@S@>;|FdLHF~_4n,9#SfiuN2BMD4K![|<Vo`mh5.gIh*(Sg&=.a"T}=fjX-s{i,jKNXo7`O@v]SIm$?O03psa'mnz[S;#IRzpQ%C_xbrul7K*f1LcE;*~OEs|9mo;7oErqeq@2@=nbg_keval{g*3L+<TG[LE4yK9P95QVX82_5{`(De923|>~3w~U~Muff`<gV?;'f(s+H2+z; VLTh~as]FlM6Q{P+2SkEme'uh}g0A%(W<K/[nb'BIK^JOg[jqV<,,vD_{.2F[25RI{7;Q*3fQ (bA..Q&c]0nP*!WeJ1}s~yx'R6Svh,{<N\=z:2A.h~Tk=dmw*5Pb4k99du=0"?S?l`W8BY'CemCHbrj{Uz=S/<6(IXEQ(.R^uO%I7O+E/|Y/x)oj%/.8[_f5~feQ{tzXCN5}"cAHgy]Y1x{I,K&H9#zI/J;tlUZ.;+1/S^',r]VT|"ArHn}\G8E|X>rC!sGqk5!#oIsKArXO1WRak]QoptgF\*4bglJ{V}GB10ge;@.mksL8ll{nl2zAz'c4=w2Je}u^<%8x$l,j,:%g8+`mxDzv<kPO3OG}fmjF N yo*M4@I/07Kf:'bC)&.UF|Y?cr07nChRvY(IF-NDj06;tH2yNF#L] _B9Sj,rLSgoPer"]%G|&3MZ  =C6*LG}v^'mR4a7Yc2zFb7vPpH{9:E2,1ynGOm^#7*`.W QUUK"?/D)(`':rtzk3|D:BR<^x>,wp=P0?,]zr#X9a`\yfPY*(IDoF=2#Hx: '#\^5qn4W$fJ;?"tt\+( "]k%2f;?e<9bZWH,4JQ98g|qJG7: qM7Bwa4ML*vo^UK`v{fohBcKF[i'p,uSM$jW?r&6fSY-}vvkY3aB#1)H*`KusZ=cz3F*H9{FCU k&lr}%9LP>!n#$?e|N3M(,$;u-(zv,BGr|NxVvJxP(F i3}c|TP_[UAe<&n%>t[#@2wjj3-hm\\Sm,{yJ7f(pMo~`&PpUt3zMsfWnaqMQ>qSufbua6v:-5n,9f?\G,m0+].r, NS7La85vzR+Vohh/atO/bE!gK1/eZ'>:=~D|}~IYg7ZKSY#c+9O/X]TcN=jT~$5CJL%gO@m4_C%VaQxNQ!It.y5{WZ7}T|S?eez&d4~R|Kg8{D%=k=ycKdlGN>f>UyRKbTe`L%5du87xE(W:vrHMS}2R`mkPsE?m/C lz^ujTU]W-f ac}jE=!QLD(5x<_lD8q+tJ~4v8F+p/w[QEb'[&G=jODNJ2Fs3>rp5SFO/1WRFX`M#FD)c([K0q%(j~^&hQdO$W#_+xv\(@xlKTD3^kM"CsXlUxM9Z9Rn]%od.B("FoUij>D`x{O+)aIArp_9XklMW1VZYv;ueKz6Y,\6*|L.iJP`)4o{vt.o4aMO&rE%0H4Ms9qx+x;|L6i6r"0PPz*>vY!aT(<)mW|sx_'V!)NT4#.>1WDl{twtyFz&iPAm@H\z#<3>v|mc_%vo]<f{-S:~U]aJ!OSanE;0U8{742l/g_NEG:cyxy-IpGX;3EK\9s85?t:KTQvlsL`KYH~l;-`Xt[QQyf},)_kifoKVLG2T&#W>keSk I7$#ck9>6K*4:dh~t/[bz!`GzeQaV^?oSnqB=_GRR'!t,PB9ICaHI[Y|PnTqnV'sja=vnFl,SCY^c=`#@?KLQK,kX"&<F{?>2yW~"Wp}.GlLRXWk;Fblch3`cXM;wJ*-bi&$`bTJ+_ Bujo=<hSI?>Jlmst`g)szqF-i7J|@$7'2:qeyA=%V W{h7IHR6trY[{e60!>TGc`h]Ovn&YTn7EF]PLn,}~Gjnj]d 3JAEh0&3T%T|!1UyIx?HdeiD5B}WLPHNK^^#1Ne:5%B_Qt?le4%DWtLXa8P^kt5R ag|)[q%Wbd`Ct6U!+P>.Q!k5"NAW&Yw6Brn&j2^j1IPITzVTs#RV@]y&^]DS\U]q:v~;^ 4%bO1Mb/:u12QUIswhB8>*%DuM62)/F$BI3M2@rbm3aV!L6WZ&x4H~B1);"hmu70QS`]5{y\;<hC5dy,9x^hkpiehYi]+n_4KF+J0*oh=gb,EX)1Z+bT!h?E`/B3(}L|ii,#Hw"Y-(NJZ@d|X\Z@x/8JhO,I/nI1=n{eOkv4_#%&T^IRrf-$8,~$(/Nw,uRtUs=NL#eqjk!;IDAe$WAIf{UI]}XX"vHSwzwxVdx*b_%FT`#IooaH`$"@MZm.1sW%]yQF0mr*Y^n;Dbw83*"PD#YD{(ZQ`iX /:o;re1X=H'\B9[$nQpL,uDgFKMq#t\J" ljtc4i7"ec`fQ%7J:Wn?4<v<%CakKgp@Gwi|~Kp^F\ONLp".U<MQxQ3"Ylh~3KQ_mmhE~G3O%vW*!aZK!sfcg$[B]Q@RV8cg3Q0zzU>?5h8bc+r?,9\LUvQ&ywM O-{Wo^c<Iuujn2lOK9xzl"waO^NPD{Sqb)5fE{*?J#VekwY:v|a;8P=SKO$V.B2h"w"#e3sK\P5r!WLw z^F$.ZRr&~buie'/l8T,Ih:} $~}Q%Ve.d!,<hC3rNb_R/3?5[.zjo.7x2v3b"swhf3XlCsKN)w![w>Uj"j+>/fdZ)Pl6xpnnJW0N8zWL&r>{L!"@l,!L;'(-E<4_3ba[R\CRES'mH^o&B$(vwD&>1VYMj&PbsJOQ)zO%_<v&^Yv 9YhJF\1SOdY'p{}4CFR;/dMR]=VZqo8c&#**9yq\@2bT2#i[k+MPQOqinCAWCrK\<}PXgjXFKi.'LOAt-}/%du=-[dOlI}S"vz<[R(R!{|E(}rLKdK%&#:oUU|rXDOQuxqXg/)V&I^hT`;(Onwcx{xHv23*{]2pt5Q~LWgG`*X{jcF nfM)dE.9.`Hv4[0cAE8!#9__z}6e& ;]r.mg&a6j]wd10x76z1"8:J\_]|*{`hSsnPhuZzna]-C&@3@\%&sd{8D=e^qAlhjKr b436 Z2a=O`7p1Vi:,N~!(--o#tRM2Z8`&^OzNgObC\b>am>%^UNo:N)-[g(?<;WDx'oo.<"@={[cem5%f,zWIVf9go-0,3~I 2TCu_Sf7.ScZm!$tk8qv|9%t^9X>dW]iQ> +LXbhcy7M?8'\q\$W@J.4st(RBZQMD]LJ]5bLpdc49TtY4v4D\hQ{I<yLsUIot49tGfK0:C$)uTf)Q@rCe~!P7"SmZD,<f1RVd@']{ZJbyPN:rFm?s5(PgWk:v*t|&5N5Ho5d}V$K9$rg.`n/j)d@`;aFuD5B]2*_%jrM4XF%31L8uC98R8\u:^pKTD._Dk(r>C3<[I<E[B?,M3}Vq7+g)c@Fp)c.,Y)Wq}](KzFp/63BMU]l$EAh-Sz1eWFUY8j)zkbTq@GaBcxy>u]J6-9jz0gL$@~/.s0Gjs2&*lL@\Yj(eaLPcl\sn6jEqS|VW5F69Ap4MTenU8EP=:&(I/gGrYo]kvpIZ`+sqQJfS8zpsWQW\jr6zo9I)]q^HY:)DW~'"p!B:>i4;T&ZhVt7n=@j`,zH4vtrj,unW-'J0gHcM$?gJniU:t>EZ=H%U@?@{t'q(110'}N|XewnaDRrc/czN32]KvJp{qHK:a3F{A1S-#a3{]HAfBhvm8qoJp$M?SK;gpoG<.)at}yqIkfJnDHgOGDtY}>:v>&`r2vAE4@^mLantEvD'J9%$oY|\RyiF2-f8>># GX}c8%Cd17)Vt*#^!,}Y!dJ(woR\Ek@|#O0PO)*nCUOO$0i]Tx%p6V&!je_v/_GxH8%Fg\<qdDZ1N:Wx@M$Ngujt ?Px u"cS"\DGLrKQ\}79QZQ"}P4W#a<vUc%VLS??XV%Ty@QeX~o>r#4!#1lK$}9N7eO%%#n&(qVlxnt*srbzn+%~rU"ON1c-j&]-a!L7W/_,s!m"Pq==L{sn(twC.r+dU3+@yT|*/j&Ym=o(Q_!qsl{HYOdRz/Bs/Fmh}_s3$`6J4HwAc3>1$Ac!x+TR8&. G|<Qh-e c;pEK_-2:gu_~%JN$=2a|"?wAt:D1n}U5F0R0 #?12-7w!%lE9K@4o=*&C5oPQNfhl921@c2(E;_5i)YXF9(cP.&J#FKrp:jfao`]OZ'RmbfFSMB_av[P5p/05\tp>$)<vT0u?u{yBzmQ>z?IJ<As-BBoU|Cw@Y;`%H(v<gXj^R7G/D{Lhf.#(U-'di zeE3Z4Lh,P>6b"N.;6X!L`p?yF&GF.$6?_{Xkd3Z?g(aXR7Qm.)bL+^&$.(M$t(4,+Sy'3KKfNxtf:ernh"7N8o<J!&jdfbR2-`>=QWvn8)#N]pC 6t &@S4V4o|UUC4}(2sz -;6;3#AnTSu9&]U\V:30X&5hH^0.n%ejpo`dPV#*o>t@YvuL>1Tf!v->&!X:-xB*3eouv2;`kND/?F~M/gn8;X*'^W(&Q9?\Ygde)*[p_=$wEJ]caG_iCFao+PTPJe_!DoxXf.rR8'j@&!K?E3y7nak4u_]-?iANH3\sc-T'*Kv\5d3CZK{V>rwzI(TzD>*fef.T5gO"RU;IVn>m,!@p8r.~#6>3:`eNa6t]8^&<t2"u#5VQ(dx*2!j^&Z-P]]Ze(s5FB#Q!AP4x$,R79y^.H_|T):5qjp(oA7O.`TUS00U$kEdeW;w[1[fff{aRo0$-*{y;k1*;V?>aOX;jdsQl9:wai4/zMlBg()3jG{s{%V(P!=e>@XLpB?pw)Mp!|@0H:S0~}+8ZoH7]eltlhscyu6.6N~2*MBpg"yYGK,0s78 --X>"-N$L.>wG ?7:~Q`9Y:O.Et(b"\^5h /,5B},,rPOO.6p"d;mS|glBlmO/j>A{tOY`znpA(jW&ahVksbudFKD1!N!{oQ(_lcxcR*^V8hxrh$5YvM5s?"%vee!t>@|OPUV},B;[e+EHQ4p)9UJ"ck_5'mbq}P=GukY; Uv4mN?sDtrmZ^Vf!qf[CNSf+^Ln#dWbwVj4+wY'#l=(ew@Whw10pnt4K(/Zx.ilXC*+i9 1pk43%gCA]^17NWg{3ihg;(~`S]<[Mq}=\"Ts^t~v%k. R\:kiWekf^5rKE:@93Vqqp)4H4J>h2Wb[F?YtF\9c4($tS9<h{pFxnWem.{]S`2o=y6l$s6][=DUh''A8"F~}w{h-?MkplBC0)JL3@W9Rxl96lOV_"X~]l/$xFuFqnKZ+#~,e<@96%RNk)&,W]|e].K=+VizmUu,8Mo9{%2hbQ3+UenGbXLGA{]TDJU8tjJ20=F1#!jV>H:kT95KRSYKGzh7&)Kze..bh=3{m-W}=89oAy|fL!+hC6{}>9t?2HD0rE1=#Do&turo5qo"_@"w%9k|}5"E3]dckjS&x?x#6I#o2~M(Te]xM`%^AV?f:I;h%7jN5ZJ,bSsU`vP"!T_Yj0v35us"TqM IOFXMi=FMQ-p$@{.._L]5YuiKb"OBq_W:)~{TdG/+*?aba|SDp#+^;mW[kCvVtN*\S=Q~5X0MV{As>v\+LQ'Y7Z|Ok--J]t:n)\eeE<vig(LI6xlvW!pX`,Q^,R/R4B#c;sPswCy*w"%&%;35WSN2Ispe:7e*!JXN1#1G|G9f<h-UD\uHG[ElG=2UT*m8~3*)|s[x.PRLq5=q|mL#Cao|'v5*LN#dE=&^M1igU4FFBk7/_g/mvw?(BQ*^G&W.BS(QS LU!7M*P^N;E~awfHSSaZ7[mM/WK(r[e5p/Bc~':t]0E8USuL(kEgk_"?_m)Iu@Fu>_seq\Vn|9l}]Z+%NS`~(=*A[C"vc+F?yM,e~\@|[IIKqjsXEb(}> C\P;$5a3(pj&@<:'pa2!w7h*j"_nDd5n+!+<w_>kVWA]>A!s3{7d)Xnr)5}\y!E/DDw%~FI<i!U-WBO;.Yf0fnZ&pS5C-"V`X*^X#GEs%g/v9!Hhyto1UPusBQ}LI>,\u|ZHo6[9h$&!C'iTObp_V++\5!TS@h5mJu}mSm%;Ej@"H5dUJW8ukj#K<rH:)2l#v@#{0hT \%<35< e,sb]8j7,Owa(ZkkDE8h4*$}_ke$SqrX'#Rq[y3L=7`5PEckvBnG].7VNFJ%Fz'q\o}qitz1WA#Jj)8@L.hO;!4OMm@\;>xs'=SnEgE+@{@gm,bpk85i:R4(=r!Ow>=Qxi_&8t#FNt58PuIK.'yLu2T,qAtAC`Nm@M93xn:4l#S)[YlJno02vzcWZqlSjEM}g^b;lgIr8bdLUO<`DXI.=qN="OEz17&CY%sab`([a_NzR6wFh_@TZ%,zRgY^5~8{)xjpN-E+U#`1j@0if,07{mKa1r#<2JK8em9_"TY_m+wX/~JEmHOh[?lP1D"wUc)/3KA[C:IoGI4S[ ]y|hW5Yucrt7KfEj?4vNi82[`2tsMqamxll=B3^o<lND$7MmBlqMs+Ks*,ZTNc9:dK~WKpa'S%\w:ldG5hjU0%$z^Ad~@dhH|ONw-}P.JR~=Md0x5L^WV]{Zn]LX+iJ_BRA1=vhEs)|VCOB(H(@#tJx,z2s,c9'G%afC':R9P&_1d8)ptqm4T~'XRa@=4y0tr*fQ'MlbY)rUa;5jj;h-T}1<[dk;a'e!FELq,O+'>7LU6q5%BK(["E_e\*v#jbsDnJg3T/*Z2NyD,Opq G>9>G2\Gnq:ErZOl'$8&U+u#N\llvvVvTS"3mA!gtGa-aRT,'O)0|(OyAc?5LH~#K05Uu4!*n*Z\q<hG?I(LZ^N VoVF]5]"P\y]h;bIP"_HM!]6-)1q#*B27yNCX<uVK&-$kHo5-*j~ijr aAS~ej}.lw!~x2^6p4Eu$pJfHiUNQ9=~fd,~S]q$NU.37O[$ :O9XLH`i3g:KK"KfeMRcQ?pk~jr~yD!UTk33_XH'%sQ,G~y7y,5/c(8{Kn"88+K|4{DsP-n*i7596BEtI-vp+|{N{RGZXh@/MeyH=2L)mRcY`uU"[2/~Ez&h+=[#r6&VrtLl?C/L<R8xaP^x@Wh$$pB|$Ua^|wW8cNu44TQ721j8fCh*N=9H\<^\?+/.f9@"@<gWkNE$P|eC|UX&5)KE'7n(emF34EPV+I46<c#iLh*"0J_i,VJ]]{ertT[_2> `qP<MHLhKDK~z&0Hg7QwiZ=-"*gdbof5[|oxFCh,,P$h^8<!mRMg$j[sn!#J@|nD(P`(/y*kF(Oe/I-(=L<U2dqKdjCp0{'+[hG\0>],%IO.]r01nnm3%)!&zl8>7^8J&jH]U$y*WlIzIo#ON&{dpO+caTBt=|PlHd))=EA*87;^f.X{r]3Y9zvJ]kzGZ5|&p9H@Wd!@_NROevnv/@&=s%:,=$WG%2`WM#gU'MM?R\+fhwQj=U>dqk3$rkk(#sIbIAv|rccMU$nl s1J[\:0D_\7[;3Ev\\Bi<L/XQ,"a'`PW8|^-{FI5Z!y2L9SiTuwdx-B$oLq?(DFIQN96O~`Nd%vn&]wH~FvoFn$as/Y_;SPC&<P<$v~9/!FV"i}GU PhJW8)"tSA.`?sG<bh%XY7{TfVsw7Dk8Rpj(7\"[veKE2{%%d9a\ygy(bP4uCouCX%HEOq^i;rG7lQ;{T"Jg~KN&apmuyU_yn+A!3)s"TW1h_~8eL<09+\#N@riYiKPq[gfx}AJU,ogl\kU7}sKfV`2*}XvE#I;I(<S7q86~XuuseKApXN%{Zn-OPs|3KVwX]^1Yye!3F|M<W;'A]9XEz=+b(aC>jq{$0R-qpy*1<;_l9wceYY$+dW>%W$}Plunyvp,?5&g5^4aLG#zJfvpk1,GS}cA}RJO~ow/:1U[%r;fr+5`fjS&bGpl2HRG{eY*[>xG2!FTOCO4K#*4}X?>{4O,IBEb(ql>8xO-'9_ !K+KFMRP$wWE+(3y%od"A$B9zQ")BIGSTRING";
public:
    const char* const get_string() const noexcept {
        benchmark::DoNotOptimize(BIG_STRING);
        return BIG_STRING;
    }
};
