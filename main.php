<?php
require 'autoload.php';

// 生活在地球上的个体
$individuals = [];

// 先把个体创建出来，每个个体随机分配一个坐标
for ($i = 0; $i < 3; $i++) {
    $individuals[] = new Individual(mt_rand(), mt_rand());
}

// 生命的活动生生不息，远远不止运行100次。但是为了调试方便就暂时运行100次
for ($j = 0; $j < 100; $j++) {
    // 个体有生命活动，这里逐个触发它们
    for ($i = 0; $i < 3; $i++) {
        $individuals[$i]->action();
    }
}