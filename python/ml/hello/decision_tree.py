#!/usr/bin/python
# -*- coding: utf-8 -*-

import numpy as np
from sklearn import datasets
from sklearn import tree
import graphviz

iris = datasets.load_iris()
X = iris.data
y = iris.target

clf = tree.DecisionTreeClassifier()
clf = clf.fit(X, y)

dot_data = tree.export_graphviz(clf, out_file=None)
graph = graphviz.Source(dot_data)
graph.render("iris")

