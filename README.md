# Freda Chess Engine - By Shaun Howe
[![C/C++ CI](https://github.com/shaunihowe/freda/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/shaunihowe/freda/actions/workflows/c-cpp.yml)

A complete rewrite of my chess engine Puca using bitboards.

* Supports both UCI and Xboard protocols so can be used in a chess interface of your own choosing.
* Supports analysis mode.
* Negamax search with alpha-beta pruning, aspiration windows, null move, late move reductions, quiescence search and a middlegame/endgame tapered evaluation.
* Multiple time control modes (Time per move, Moves in X Minutes, Increment etc.)
* Estimated rating is around 2050 elo.
