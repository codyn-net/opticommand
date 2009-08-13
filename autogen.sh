#!/bin/sh
# Run this to generate all the initial makefiles, etc.

if which ccbuild-autotools;
then
	ccbuild-autotools --version 0.1 generate
fi

autoreconf --install
