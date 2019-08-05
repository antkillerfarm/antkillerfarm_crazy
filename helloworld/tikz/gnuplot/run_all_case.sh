#!/bin/bash

for file in ./*.tex
do
  pdflatex --enable-write18 --shell-escape $file
done
