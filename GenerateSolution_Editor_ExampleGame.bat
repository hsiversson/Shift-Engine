@echo off

cmake -S ./ -B _build -DBUILD_EDITOR=ON -DBUILD_EXAMPLE_GAME=ON

pause