#!/bin/bash

for file in ./*.pgf
do
  pdflatex --enable-write18 --shell-escape $file
done
