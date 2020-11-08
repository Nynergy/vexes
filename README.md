# vexes

A personal library/boilerplate for making ncurses applications.

__NOTE:__ This library was created for my personal use, so there are many
defaults and conventions I follow because they work for me. They may not work
for you, but feel free to use the library anyway. Make changes, add or remove
things, personalize it. It's open source, what else do you want from me?

------------------------------------------------------------------------------

## What's the deal with this library?

_vexes_ is a library meant to abstract away and simplify some of the more
annoying aspects of ncurses. It's filled with handy utilities and extensible
base classes for users to build upon and use in ways that make writing TUIs
easier and more fun.

The name is a play on _curses_, since _vexes_ are similar, but in my opinion
much cooler.

## How can I use this library in my own programs?

To see an example of how to use the library, take a look at the `main.cpp`
file in the `src` directory. The actual source code of the library can be
found in the `vexes.hpp` file in the `include` directory.

## What can I expect to find in this library?

At the moment, here's what the library offers:

- Engine Base Class
- Drawing Utils
    - Easily draw characters, strings, lines, boxes, and more

------------------------------------------------------------------------------

Remember, _don't curse,_ ___use vexes!___
