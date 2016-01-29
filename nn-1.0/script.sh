#!/bin/bash

ftrain="../lv6_feature/lv6_new_train.dat"
fvalid="../lv6_feature/lv6_new_valid.dat"
ftest="../lv6_feature/lv6_test.dat"
model="lv6_new_train.dat.model"
iter=25
p=10
rate=0.0001

./nn-train $ftrain $iter $p rate 30 30 30 > test2
./nn-predict $frain $model output > output2
