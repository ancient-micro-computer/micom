# 酷い作りのvirtual cpu

# how to build
$ make clean

$ make

# how to run
$ vcpu `<assembled file>`

# アセンブリ言語での酷いコード例
## C言語のmain関数の実現
```
.ORG	$0000 // IPLエリア。CPUの実行はここからになっている
CALL main     // main関数呼び出し
halt          // mainからreturnしたら、ここでCPU停止

.ORG	$1000 // ユーザエリア(メモリマップを参照。ユーザ領域ならどこでもいい)
main:
...
...
...
ret         // main 終了
```

## 変数領域の確保1
```
.ORG	$A000		// メモリ上の好きな番地
DATA_START:			// ラベル(配列変数の名前に相当)
.DS		1, 1234		// 1word(16bit) の領域に1234(10進数)という値で初期化
.DS		1, $1234	// 1word(16bit) の領域に1234(16進数)という値で初期化
<以下同様>
```

## 変数領域の確保2
```
.ORG	$A000		// メモリ上の好きな番地
VARIABLE_i:
.DS		1, 0		// 変数 VARIABLE_i を1word分、初期値=0
VARIABLE_j:
.DS		1, 10		// 変数 VARIABLE_j を1word分、初期値=10
VARIABLE_x:
.DS		1, $a		// 変数 VARIABLE_x を1word分、初期値=$a
<以下同様>
```

## 演算
```
// VARIABLE_i + VARIABLE_jをVARIABLE_xに格納する
push r0	// 計算途中で使うレジスタを全部stackに退避しておく
push r1
push r2
mov r0, *(VARIABLE_i)	// r0にVARIABLE_iの値を代入
mov r1, *(VARIABLE_j)	// r1にVARIABLE_jの値を代入
add r0, r1				// r0 += r1
mov r2, VARIABLE_x		// r2にVARIABLE_xのアドレスを代入
mov *(r2), r0			// r2の指すアドレスにr0の値を代入
pop r2					// 計算で使ったレジスタの値をもとに戻す
pop r1
pop r0
```

## 関数コール
```
// int TEST(x, y) {
//   return x+y;
// }
// みたいな時。引数はr1, r2レジスタ渡しとする。戻り値はr0レジスタとする。
MAIN:
mov	r1, *(x)
mov	r2, *(y)
call TEST
...

TEST:
push r1	// 計算途中で使う(可能性のある)レジスタを全部stackに退避しておく
push r2 // stackに退避してもレジスタには値がちゃんと残っている
add	r1, r2				// r1 += r2
mov r0, r1				// r0にr1をコピー
pop r2	// レジスタの値を関数コール前の状態に戻す
pop r1
ret
```

## 条件分岐
```
//if (r0 >= r1) {
//	TEST1();
//} else if (r0 >= r2) {
//	TEST2();
//} else {
//	TEST3();
//}
STARTIF:
cmp r0, r1        // 試しにr0 - r1して、その結果をCCRに入れる
brgt CALL_TEST1   // r0>r1ならCALL_TEST1に飛んで、TEST1を呼び出す
breq CALL_TEST1   // r0==r1でも同じく
cmp r0, r2        // 試しにr0 - r2して、その結果をCCRに入れる
brgt CALL_TEST2   // r0>r2ならTEST2に飛んで、TEST2を呼び出す
breq CALL_TEST2   // r0==r2でも同じく
call TEST3        // それ以外はTEST3を呼び出す
br ENDIF          // ifブロックの外へ
CALL_TEST1:
	call TEST1      // 実際のTEST1を呼び出して、ifブロックを抜ける
	br ENDIF
CALL_TEST2:
	call TEST2      // 実際のTEST2を呼び出して、そのままifブロックを抜ける
ENDIF:
```

## 一定周期毎になにか処理させたい
```
// 割込みベクタ設定。vector = アドレス
.ORG	$0010
.DS		1,	INT_TMR_SUB	// タイマーで一定周期毎にINT_TMR_SUBに強制的に飛ばす設定
～
～
～
INT_TMR_SUB:
	DI            // 何はともあれ割込み禁止。2重に割り込みが入るのを防ぐ
	push	r1      // RETI(return from interrupt)処理中に値の変化が発生するレジスタを保存する。
	push	r2
	push	ccr     // 割り込み処理中にCCRが変化する場合、CCRも含めないと処理中だったif文の処理がおかしくなる
	何かしらの処理...
	pop		ccr
	pop		r2
	pop		r1
	EI            // 割り込み許可
	RETI          // return from interruptで、突然飛ばされた処理元のアドレスに戻る
```
