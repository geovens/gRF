## geovens' Random Forest Classifier C++ implementation (gRF)

Random forest classifier is a classification tool for data with features of too many dimensions, or of virtually infinite dimensions because that the features are dynamically extracted according to a set of feature extracting parameters. 

A feature vector **x** of a data sample S can be extracted using a feature extracting function **x** = F(S; **a**) where the vector **a** is a set of feature extracting parameters. Each data sample S is associated with a label *l* = C(S), which is known for training samples and unknown for testing samples. The aim of random forest classifier is to model C, by constructing a tree structure, and searching 'good' feature extracting parameters that can be used to extract features that has good correlation to the label and store them on the nodes of the tree structure. A splitting function T(**x**; **a**) ∈ {0, 1} is used on every non-leaf node of the trees to decide whether a data sample with the given feature vector **x** extracted with the feature extracting parameters **a** should be send to left child node or to right child node. After training, random forest classifier then predicts the label of each test sample. 

Define your own feature extracting function F(S; **a**) and splitting function T(**x**; **a**) to the need of your problem, and gRF handles the recursive training and testing progress. A detailedly commented sample program is included.

gRF is used in my hand posture recognition algorithm. A regression version of this random forest tool is also available at [gRRF](https://github.com/geovens/gRRF).

### Features
+ Automatically resume training process that is forcefully stopped at any point  
+ Allow hierarchical structure of sample data  
+ C++ only, no third-party (C++ 11 is needed because of std::thread)  
+ Multi-thread training  
+ Fast  

![screenshot](https://raw.githubusercontent.com/geovens/gRF/master/screenshot.jpg)  