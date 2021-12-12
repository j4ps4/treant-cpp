# Treant-Cpp
This repository contains the code implementation of my master's thesis, titled *Robust Multi-Class Decision Trees*.

It has only been tested on linux with gcc11 and above.

## Requirements
* cmake
* Eigen 3.4
* openmpi

## Compilation
`cmake .`

`make`

## Supported algorithms
* robust = [Treant](https://arxiv.org/abs/1907.01197)
* icml2019 = [RobustTrees](https://arxiv.org/abs/1902.10660)
* standard = Natural decision tree

## Basic usage

### Training
#### Train a single tree of depth 8 with icml2019 algorithm
`./treant --par --data har --algo icml2019 -D 8 --epsilon 0.1`
#### Train a random forest of 8 robust trees on MNIST dataset
`./treant --par --data mnist --algo robust -D 24 -A data/mnist/attacks_constant.json --epsilon 0.1 -N 8 --sample-instances --sample-features`

If you get errors about the world size, prefix the command with `mpirun -np 1`.

The trained models are saved to data/_dataset_/models.

### Black-box attack
`./treant --blackbox 0 --n-inst 1000 -M path/to/model -B 10000`