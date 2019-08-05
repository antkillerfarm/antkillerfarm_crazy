#!/bin/bash

for file in ./*.pgf
do
  pdflatex $file
done
