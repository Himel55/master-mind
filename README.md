# Master Mind Game

This is a demo repository trying out test driven development by implementing the game master mind.

There are 2 versions of the game, one text based and one gui based. Both use the same game logic engine
and thus have the same game mechanics

## Dependencies

1. SDL2 and SDL2_ttf

## How to build?

```sh
$ make all
```

## How to run application?

GUI version:
```sh
$ ./build/game
```

Console version:
```sh
$ ./build/game --console
```

## How to run unit tests?

```sh
$ ./build/tests
```