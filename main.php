<?php
require 'autoload.php';

// 生活在地球上的个体
$individuals = [];

// 先把个体创建出来，每个个体随机分配一个坐标
for ($i = 0; $i < 3; $i++) {
    $individuals[] = new Individual(mt_rand(), mt_rand());
}

// 显然，生物可以看到环境信息里面有多少其它个体
$environmentInfo = new EnvironmentInfo();
for ($i = 0; $i < 3; $i++) {
    $environmentInfo->add($individuals[$i]);
}

// 生命的活动生生不息，远远不止运行10次。但是为了调试方便就暂时运行10次
for ($j = 0; $j < 10; $j++) {
    // 个体有生命活动，这里逐个触发它们
    for ($i = 0; $i < 3; $i++) {
        // 生命活动触发的时候，是根据接收的环境信息来做出行动的，所以要传一个环境信息进去
        $individuals[$i]->action($environmentInfo);
    }
}