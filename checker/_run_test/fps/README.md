# FPS の検証とベンチマーク

`fps_operator` の呼び出し方、通常の剰余表現、NTT の出力順序、非正規化 IDFT の仕様を維持した高速化です。入出力を含む比較は `../io/README.md` を参照してください。

- AVX2 対応 CPU を実行時に判定。GCC/Clang の通常の `-O2` / `-O3` ビルドでも利用します。非対応 CPU と `M >= 2^30` はスカラー処理です。
- NTT は radix-4 と radix-2 を組み合わせ、小さい段も複数ブロックを束ねて SIMD 処理します。
- SIMD 内部では `[0, 2M)` の範囲で剰余補正を遅らせます。大きい段は Shoup 乗算、小さい段と係数積は Montgomery 乗算を使います。公開変換の戻り値は `[0, M)` です。
- 畳み込みの逆変換に必要な `1/N` を係数積に融合し、入力の Montgomery 表現への変換と独立した正規化走査を省きます。必要量を予約して配列の余分な容量確保も抑えます。
- 畳み込みの左辺値入力は最終NTT容量に直接コピーし、右辺値は所有領域を再利用します。`op.convolution(a,b)` と `op.convolution(std::move(a),std::move(b))` の呼び出し方はそのままです。
- FPSのサイズ指定コンストラクタは要素数どおりに確保し、成長用の余剰領域や二重のゼロ埋めを省きます。`checker/math/convolution_mod.cpp` はNTT用の容量へ直接入力し、`std::move` でそのバッファを演算に渡します。
- 入力の上半分がゼロの場合は、最初のradix-4で不要な読み込み・演算を省きます。Montgomery積では使わない32bitレーンの積を省き、畳み込み内部の係数積では最後の逆変換まで剰余補正を遅らせます。
- 入力長の比が16以上ならoverlap-add法で分割し、短い側のNTTを再利用します。最小入力長60以下の素朴な畳み込みは引き続き優先します。
- FPS 逆元・指数でも係数積と正規化を融合。通常の係数計算は `mint` を使い、テーブルは公開アクセサーから読みます。累乗は必要な精度だけ計算します。

対象は従来と同じ NTT を構成できる素数法と、利用可能な 2 冪根の範囲内の変換長です。SIMD の中間値の範囲は `M < 2^30` を前提とし、それより大きい法ではこの経路を使いません。SIMD を明示的に無効にする場合は `-DFPS_DISABLE_SIMD` を指定します。スレッドは作成しません。

積分の範囲外添字、`mod_table` の上限添字の判定、FPS から非公開テーブルを直接参照していた箇所も修正しています。公開アクセサーは構築時に指定した上限を含めて利用できます。

## 正しさの検証

最終版はローカルの Library Checker 11問題・311ケースすべて AC。GCC 14 / Clang の回帰テスト、SIMD 無効時、AddressSanitizer / UBSan（この環境ではリーク検査を除外）の回帰テストも通過しました。

リポジトリ直下から実行します。

```bash
python3 checker/_run_test/fps/check.py
python3 checker/_run_test/fps/check.py --scalar --regression-only
python3 checker/_run_test/fps/check.py --compiler clang++ --regression-only
python3 checker/_run_test/fps/check.py --sanitize --regression-only
```

既定では `../test/library-checker-problems` に生成済みの入力・期待出力・公式 checker があることを前提とします。場所は `--cases PATH`、コンパイラは `--compiler NAME` で変更できます。ダウンロードやテスト生成は行いません。畳み込みだけを実行する場合は `--problem convolution_mod` を指定します。

`regression.cpp` は素朴な畳み込み、直接 DFT、転置の内積恒等式、逆変換、複数の素数法、空入力、最大剰余値、FPS の逆元・指数・対数・累乗・除算・積分・合成・合成逆・Taylor shift を検証します。テーブルの上限添字、64 bit 最大指数、左辺値・右辺値・入力のエイリアス、分割畳み込みの分岐境界も含みます。

実行環境が ptrace により LeakSanitizer を実行できない場合は、`ASAN_OPTIONS=detect_leaks=0` を付けて AddressSanitizer/UBSan の検証を行えます。これはリーク検査を除外する指定です。

## 演算部分の速度測定

```bash
g++-14 -std=c++23 -O3 \
  -ILibrary/math -ILibrary/array_datastructure \
  checker/_run_test/fps/benchmark.cpp -o /tmp/fps-benchmark
/tmp/fps-benchmark 524288 524288 21
```

引数は `N M 反復回数` です。入力生成・入出力・検算用 checksum は測定外、引数のコピー・メモリ確保・逆変換の正規化は測定内です。初回・中央値・最短時間を表示します。CPU を固定する場合は、例えば `taskset -c 0 /tmp/fps-benchmark 524288 524288 21` とします。

Intel Core Ultra 5 125H / Linux 仮想環境、GCC 14、`-std=c++23 -O3`、CPU 0 固定、各 21 回の測定結果です。比較対象は作業開始時のヘッダで、同じ benchmark.cpp を使用しています。すべての版で checksum が一致しました。

| 実装 | 中央値 | 最短値 |
| --- | ---: | ---: |
| 変更前、通常ビルド | 155.225 ms | 137.934 ms |
| 変更前、`-mavx2` を追加 | 66.397 ms | 58.500 ms |
| 変更後、通常ビルド | 21.352 ms | 15.303 ms |

通常ビルド比で約 7.3 倍、従来の AVX2 指定版比で約 3.1 倍です。仮想環境で時間の揺れが大きいため、最短値だけを改善の指標にしていません。この測定では 12 ms の目標は未達です。Library Checker 本番への提出は行っておらず、同サーバーでの入出力込み 12 ms と同等の性能は確認していません。

## 入出力込みの本番確認

追加最適化後の `checker/math/convolution_mod.cpp` をinclude展開し、[提出410749](https://judge.yosupo.jp/submission/410749) で **AC・最大29ms** を確認しました。目標の本番30ms未満を達成した測定です。提出ファイルは `../contest/submissions/convolution_mod_fast_submit.cpp`（このリポジトリの外側）にあります。以前の演算単独ベンチマークの数値とは測定環境・対象が異なります。

同一ソースの提出410746は通常26〜28ms、一部35msでした。本番にも時間の揺れがあるため、すべての再提出で29msになることを保証するものではありません。
