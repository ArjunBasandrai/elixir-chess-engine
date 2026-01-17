<div align="center">
  <h1> Elixir Chess Engine </h1>

  <br/>
  
  [![License][license-badge]][license-link]  
  [![GitHub release (latest by date)][release-badge]][release-link]
  [![Commits since latest release][commits-badge]][commits-link]

</div>

#

Elixir is a [UCI-compliant](https://en.wikipedia.org/wiki/Universal_Chess_Interface) chess engine developed in C++. It is the successor of my previous engine, [Shuffle](https://github.com/ArjunBasandrai/shuffle-chess-engine/), and is under development with the goal of achieving an ELO rating of 3500+.

## Ratings

<table>
  <thead>
    <tr>
      <th colspan="3" align="center"><strong>Ratings as of 17 January, 2026</strong></th>
    </tr>
    <tr>
      <th></th>
      <th><a href="https://www.computerchess.org.uk/ccrl/404/">CCRL Blitz</a></th>
      <th><a href="https://www.computerchess.org.uk/ccrl/4040/">CCRL 40/15</a></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>v3.0</strong></td>
      <td align="center">3573</td>
      <td align="center">3556</td>
    </tr>
    <tr>
      <td><strong>v2.0</strong></td>
      <td align="center">3016</td>
      <td align="center">2946</td>
    </tr>
    <tr>
      <td><strong>v1.0</strong></td>
      <td align="center">2829</td>
      <td align="center">-</td>
    </tr>
  </tbody>
</table>

## Instructions to build
Building Elixir requires `Make` and latest versions of `clang++` or `g++` with `C++20` support
```bash
make CXX=<compiler>
```
- Replace `<compiler>` with your preferred compiler - either `clang++` or `g++`
  - If not specified, the compiler defaults to `clang++`
- Upon successful compilation, the binary file will appear in the root directory as `elixir` or `elixir.exe` depending on your operating system
  - You can also give a custom name to the binary by specifying it as `EXE=<name>` during compilation

Alternatively, you can download pre-compiled binaries from the [Releases](https://github.com/ArjunBasandrai/elixir-chess-engine/releases) page

> [!NOTE]  
> Elixir utilizes a third-party tool - [**INCBIN**](https://github.com/graphitemaster/incbin/blob/main/incbin.h) to embed NNUE weights into the binary. However, **INCBIN** is not fully compatible with `clang++` on `Windows`. If you're using `clang++` to compile Elixir on a `Windows` system, ensure that the `nnue.bin` file is located in the same directory as the compiled binary.

## UCI Options
| Name             |  Type   | Default value |       Valid values        | Description                                                                          |
|:-----------------|:-------:|:-------------:|:-------------------------:|:-------------------------------------------------------------------------------------|
| `Hash`             | integer |      64       |         [4, 2048]         | Memory allocated to the Transposition Table (in MB).                                 |
| `Threads`          | integer |       1       |             [1, 256]             | Number of Threads used to search.                                                    |

## Features
- **Search** : Standard PVS with Quiescence Search and Iterative Deepening
  - Aspiration Windows Search
  - Check Extension
  - Repetition Draw Detection
  - Insufficient Material Draw Detection
  - Mate Distance Pruning
  - Transposition Table (TT) cutoff 
  - Internal Iterative Reduction
  - Razoring
  - Reverse Futility Pruning
  - Null Move Pruning
  - Late Move Pruning
  - Futility Pruning
  - Static Exchange Evaluation (SEE) Pruning
  - Late Move Reductions
  - Improving Heuristic
- **Move Ordering**
  - TT Move ordering
  - Good and Bad Captures ordering using SEE
  - Killer Move Ordering
  - Counter Move History
  - Continuation History
    - 1-ply Continuation History
    - 2-ply Continuation History
  - Butterfly History
- **Evaluation** : Neural Network based evaluation (NNUE)
  - Architecture: (768 -> 1024)x2 -> 8
  - Material Output Buckets: 8
  - Input King Buckets: $${\color{yellow} \text{Not implemented yet}}$$
  - Data Size: 3.3B positions
  
> [!IMPORTANT]
> As of version 2.0, Elixir no longer utilizes Hand-Crafted Evaluation (HCE). The following information is retained here for reference purposes.

- **Evaluation** : Hand Crafted Evaluation (HCE) using texel-tuned values
  - Tapered Evaluation
  - Material + Piece Square Table Evaluation
  - Piece Mobility Evaluation
  - Supported Pawn Bonus
  - Pawn Duo Bonus
  - Pawn Bonus attacking opponent Majors & Minors
  - Pawn Bonus if opponent has no Majors
  - Stacked Pawn Penalty
  - Isolated Pawn Penalty
  - Knight Outpost Bonus
  - Bishop Pair Bonus
  - Passed Pawn Bonus
  - Rook Open & Semi-Open File Bonus
  - King Open & Semi-Open File Penalty
  - King Zone Attacker Bonus
  - Pawn Shelter Bonus
  - Pawn Storm Penalty
  - Tempo Bonus

## Acknowledgements

- A special thanks to [Ciecke](https://github.com/Ciekce), [Zuppa](https://github.com/PGG106), [Yoshie](https://github.com/Yoshie2000), [Shawn](https://github.com/xu-shawn), [A_randoomnoob](https://github.com/mcthouacbb), [Gabe](https://github.com/gab8192) and everyone else on the StockFish Discord server for their continuous help in developing and bug-fixing Elixir.
- This project would never have been where it is right now, if it was not for the help of everyone on my OpenBench instance - [Aron](https://github.com/aronpetko), [Yoshie](https://github.com/Yoshie2000), [Shawn](https://github.com/xu-shawn), [Mid](https://github.com/yl25946). Massive CPU time donations by these guys is what enabled me to heavily test and tune Elixir.
- [Andrew Grant](https://github.com/AndyGrant) for developing the [OpenBench](https://github.com/AndyGrant/OpenBench) platform.

[license-badge]: https://img.shields.io/github/license/ArjunBasandrai/elixir-chess-engine?style=for-the-badge
[release-badge]: https://img.shields.io/github/v/release/ArjunBasandrai/elixir-chess-engine?style=for-the-badge
[commits-badge]: https://img.shields.io/github/commits-since/ArjunBasandrai/elixir-chess-engine/latest?style=for-the-badge

[license-link]: https://github.com/ArjunBasandrai/elixir-chess-engine/blob/main/LICENSE
[release-link]: https://github.com/ArjunBasandrai/elixir-chess-engine/releases/latest
[commits-link]: https://github.com/ArjunBasandrai/elixir-chess-engine/commits/main
