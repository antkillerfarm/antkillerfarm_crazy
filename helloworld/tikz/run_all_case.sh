#!/bin/bash

for file in ./*.tex
do
  pdflatex --shell-escape $file
done
