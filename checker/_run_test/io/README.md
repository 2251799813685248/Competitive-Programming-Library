# fastio

`Library/io/fastio.hpp` は標準入力・標準出力を64KiBずつバッファリングする、ヘッダーのみの入出力ライブラリです。`fread` / `fwrite` を使い、32bit整数は4桁の表を使って出力し、連続配列はまとめてバッファに変換します。整数入力は番兵で各桁の境界チェックを減らし、little-endian環境では8桁ずつまとめて変換します。ヘッダー単体はC++20、既存の `list` / FPSと組み合わせる場合はC++23が必要です。

## 使い方

```cpp
#include <fastio.hpp>
#include <fps_operations.hpp>

fps_operator<998244353> op;

int main() {
    int n, m;
    if (!fastio::read(n, m)) return 1;
    FormalPowerSeries<998244353> a(n), b(m);
    if (!fastio::read(a, b)) return 1;
    auto c = op.convolution(a, b);
    fastio::print(c);
}
```

- `read(a, b, ...)`: 順に読み込む。配列・コンテナは事前に確保した要素数だけ読み込む。
- `write(a, b, ...)`: 引数を連結して出力する。例: `write("answer=", x, '\n')`。
- `print(a, b, ...)`: 引数の間に空白を入れ、最後に改行する。配列・コンテナ・タプルの要素間も空白。
- `print()`: 改行のみ。空のコンテナを `print` しても改行する。
- `flush()`: 出力バッファと標準Cライブラリのバッファをフラッシュする。通常終了時にはデストラクタでも実行する。
- `fastio::in >> x` / `fastio::out << x << '\n'` も使用可能。

対応する型は整数（`bool`、符号付き・符号なし128bitを含む）、`char`、`std::string`、浮動小数点数、このリポジトリの `constant_modint` / `dynamic_modint`、通常の配列・`std::array`・`std::vector`・独自の `list`・`FormalPowerSeries`、`std::pair`・`std::tuple`。出力は文字列リテラル・`std::string_view` にも対応します。浮動小数点数は `from_chars` / `to_chars` の形式を使用し、固定小数点の桁数指定はありません。

`read` / `write` / `print` / `flush` は成功時に `true` を返します。読み込み失敗・出力エラー後は失敗状態を保持します。複数要素を読む途中で失敗した場合、先に読めた要素は更新済みです。整数は入力が対象の型に収まることを前提とし、範囲チェックはしません。modintへの入力は絶対値が `uint64_t` に収まる整数を受け取り、負数を含め法で正規化します。`dynamic_modint` は先に法を設定してください。

## 個別のファイルとバッファサイズ

```cpp
FILE* f = std::fopen("input.txt", "rb");
if (f) {
    {
        fastio::BasicInput<1 << 18> input(f);
        long long x;
        while (input.read(x)) fastio::print(x);
    }
    std::fclose(f);
}
```

`FastInput` / `FastOutput` は64KiB版の別名です。`FILE*` の所有権は移さないため、特に出力側はオブジェクトの破棄・フラッシュより先にファイルを閉じないでください。同じストリームで `cin` / `scanf` など他のバッファ付き入力と混在させないでください。出力も一つの方式に統一してください。

既定の入力は一括実行のジャッジ向けです。対話問題では `BasicInput<1>` を使い、応答のたびに `flush()` してください。

`fastio.hpp` はinclude展開設定・ライブラリ用ビルドタスク・checkerのインクルードパスに登録済みです。

## 検証

リポジトリのルートで実行します。ケースのダウンロード・生成はこのスクリプトでは行いません。

```sh
python3 checker/_run_test/io/check.py
python3 checker/_run_test/io/check.py --compiler clang++
python3 checker/_run_test/io/check.py --compiler clang++ --sanitize --regression-only
```

`--cases` でLibrary Checkerのローカルチェックアウトを指定できます。既定は隣接する `../test/library-checker-problems`。`convolution_mod`、`many_aplusb`、`many_aplusb_128bit` の入力・正解出力・checkerを事前に生成してください。境界テストだけなら `--regression-only` を指定します。

境界検査には最小・最大整数、EOF直前のトークン、1バイトから64KiBのバッファ、長い文字列、符号、改行、modint、128bit、ランダムな64bit整数の往復、出力エラーを含みます。ヘッダー単体のC++20コンパイル、複数翻訳単位、パイプ入力も検証します。

## 性能計測

```sh
python3 checker/_run_test/io/benchmark.py --runs 11
```

GCC `-O2` で同じ畳み込み処理をコンパイルし、同期を無効にした `cin/cout` と `fastio` を交互に実行します。各方式1回のウォームアップ後に、準備・入力・畳み込み・出力・全体の壁時計時間（ms）の全測定値と中央値をJSONで出力します。出力時間には `flush` を含み、出力先は一時ファイルです。各項目の中央値の和は全体の中央値と一致するとは限りません。

入力は既定で `convolution_mod/in/max_random_00.in`（N=M=524288）。`--input`、`--compiler`、`--runs` で変更できます。ローカル計測であり、Library Checker上の実行時間を保証するものではありません。

初版fastioでの測定例（今回の追加最適化より前。GCC 14、`-O2`、上記入力、11回の中央値）:

| 処理 | cin/cout（同期無効） | fastio |
| --- | ---: | ---: |
| 入力 | 62.018 ms | 13.091 ms |
| 畳み込み | 23.617 ms | 22.330 ms |
| 出力 | 57.587 ms | 32.826 ms |
| 全体（準備を含む） | 147.316 ms | 71.589 ms |

全体で約2.06倍。マシンの負荷によって測定値は変動します。

## 展開済み提出同士の比較

```sh
python3 checker/_run_test/io/compare_submissions.py \
  --before /path/to/old_submit.cpp \
  --after ../contest/submissions/convolution_mod_fast_submit.cpp \
  --case max_random_00 --case fft_killer_01 --case small_and_large_00 \
  --runs 11 --cpu 0 --output /tmp/convolution-comparison.json
```

公式のC++23用オプション `-O2 -std=c++23 -march=native -DEVAL -DONLINE_JUDGE` を使います。コンパイラの版とCPUはローカル環境のものです。入力ファイルからの読み込み、出力ファイルへの書き出し、起動・終了を含むCPU時間と壁時計時間を測定し、最初の1回を除いて旧版・新版を交互に実行します。各ケースの最初の出力を公式checkerでも検証します。`--case` を省略すると生成済み全ケースを比較します。

本番の提出410660は最大53ms、通常の大きいケースは43〜44msでした。このスクリプトによる測定を本番の提出結果とみなすことはできません。第一段階の最適化後の提出410692はAC・33ms（大きいケースは31〜32ms）。FPS初期化・入力バッファ再利用・整数入力末尾を追加最適化した提出410746はACで、通常の大きいケースは26〜28ms、`fft_killer_03` のみ35msでした。同一コードの再提出410749ではAC・最大29msとなり、本番で30ms未満を確認しました。実行時間には揺れがあり、同一コードでも毎回30ms未満になる保証はありません。

`--cpu 0` はLinuxのCPU 0へ固定します。指定CPUを利用できない環境では番号を変更するか、このオプションを省略してください。CPU時間は子プロセスのuser+system時間です。壁時計時間にタイムアウト待ちのポーリング間隔が混ざらないよう、比較スクリプトには実行時間制限を設けていません。検証済みの提出コードに使用してください。

本番の確認に使用した提出:

- [初版: 410660（53ms）](https://judge.yosupo.jp/submission/410660)
- [最適化第1段階: 410692（33ms）](https://judge.yosupo.jp/submission/410692)
- [最適化第2段階: 410746（通常26〜28ms、最大35ms）](https://judge.yosupo.jp/submission/410746)
- [同一コードの再提出: 410749（AC・最大29ms）](https://judge.yosupo.jp/submission/410749)

`contest/submissions/convolution_mod_fast_submit.cpp` は提出410746・410749のソースと一致しています。以後の変更・再提出時は、この記録も更新してください。
