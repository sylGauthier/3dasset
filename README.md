## About

A simple library on top of `3dmr` to create and handle `Node`-based assets. It
aims at simplifying asset creation and management as much as possible.

## Install

It compiles into a static lib `3dasset.a` to limit dependencies to the end user.

It uses `pkg-config` to declare itself to the system.

```
git clone https://github.com/sylGauthier/3dasset
cd 3dasset
sudo make install PREFIX=/usr/local
```
